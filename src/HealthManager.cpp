#include "HealthManager.h"
#include "detail/TextIO.h"
#include <algorithm>
#include <iomanip>
#include <set>
#include <sstream>
/**
 * @brief 查找第一个未使用的M前缀医疗编号。
 * 参数：无。
 * @return 例如M001；超过三位时自然扩展。
 */
std::string HealthManager::generateRecordId() const {
    for (std::size_t n = 1;; ++n) {
        std::ostringstream s;
        s << 'M' << std::setw(3) << std::setfill('0') << n;
        bool used = false;
        for (const auto &r : records)
            if (r->getId() == s.str())
                used = true;
        if (!used)
            return s.str();
    }
}
/**
 * @brief 检查动物在园性及医疗编号，再插入记录并执行可选状态变更。
 * @param r [in] 待接管的非空医疗unique_ptr。
 * @param status 可选目标状态；调用者负责限定可用枚举。
 * @return 全部成功true；校验失败false；状态更新失败撤销本条记录。
 * @note 接管指针；不自动保存；死亡/转出档案只能查询旧医疗历史。
 */
bool HealthManager::append(std::unique_ptr<MedicalRecord> r, std::optional<AnimalStatus> status) {
    lastError.clear();
    // 保留历史档案，但禁止为已转出/死亡动物登记新的在园医疗行为。
    if (!animals.canReceiveFeeding(r->getAnimalId())) {
        lastError = "动物不存在、已转出或已死亡";
        return false;
    }
    for (const auto &old : records)
        if (old->getId() == r->getId()) {
            lastError = "医疗编号重复";
            return false;
        }
    // 先完成可能分配内存的插入，再修改状态；状态已由各接口限定。
    const auto id = r->getAnimalId();
    records.push_back(std::move(r));
    if (status && !animals.changeAnimalStatus(id, *status)) {
        records.pop_back();
        lastError = "状态更新失败，已撤销本条记录";
        return false;
    }
    return true;
}
/**
 * @brief 复制并登记体检记录，保持动物当前状态。
 * @param r 已通过构造校验的体检记录；Manager再检查编号和动物。
 * @return 登记成功true，否则false。
 * @note 只登记在园动物；记录副本由本Manager管理。
 */
bool HealthManager::addHealthCheck(const HealthCheckRecord &r) {
    return append(std::make_unique<HealthCheckRecord>(r));
}
/**
 * @brief 登记疾病，可选择同时改为治疗中或隔离。
 * @param r 待复制的疾病记录。
 * @param s 可选目标状态；nullopt只记录；仅接受TREATMENT/ISOLATED。
 * @return 记录与可选状态均成功true，否则false。
 * @note 不允许将不存在、转出或死亡动物登记为新的在园医疗行为。
 */
bool HealthManager::addDiseaseRecord(const DiseaseRecord &r, std::optional<AnimalStatus> s) {
    if (s && *s != AnimalStatus::TREATMENT && *s != AnimalStatus::ISOLATED) {
        lastError = "疾病状态只能选择治疗或隔离";
        return false;
    }
    return append(std::make_unique<DiseaseRecord>(r), s);
}
/**
 * @brief 登记治疗方案记录，不自动改变当前状态。
 * @param r 待复制的治疗记录。
 * @return 成功true；动物或编号不合法false。
 */
bool HealthManager::addTreatmentRecord(const TreatmentRecord &r) {
    return append(std::make_unique<TreatmentRecord>(r));
}
/**
 * @brief 登记康复记录，并按选择恢复正常展出状态。
 * @param r 待复制的康复记录。
 * @param normal true恢复NORMAL；false只记录，默认true。
 * @return 记录及可选状态更新均成功true，否则false。
 * @note 转出或死亡动物不能通过此接口重新变成在园正常状态。
 */
bool HealthManager::addRecoveryRecord(const RecoveryRecord &r, bool normal) {
    return append(std::make_unique<RecoveryRecord>(r),
                  normal ? std::optional<AnimalStatus>(AnimalStatus::NORMAL) : std::nullopt);
}
/**
 * @brief 查询某动物全部医疗历史，并按日期稳定排序。
 * @param id 待查询动物编号。
 * @return 医疗记录副本列表；无记录/未知动物为空；同日期保持登记先后顺序。
 */
std::vector<MedicalRecordInfo>
HealthManager::getMedicalRecordsByAnimal(const std::string &id) const {
    std::vector<MedicalRecordInfo> out;
    for (const auto &r : records)
        if (r->getAnimalId() == id)
            out.push_back(r->toInfo());
    /**
     * @brief 日期比较回调：按日期升序排序，同日顺序保持不变。
     * @param a 左侧医疗副本。
     * @param b 右侧医疗副本。
     * @return a.date早于b.date时true。
     */
    std::stable_sort(out.begin(), out.end(),
                     [](const auto &a, const auto &b) { return a.date < b.date; });
    return out;
}
/**
 * @brief 从6字段医疗文件加载历史，成功后整体替换医疗列表。
 * @param file UTF-8医疗文件路径；须先加载动物。
 * @return 全部记录合法true；文件/字段/关联错误false。
 * @note 失败保留旧记录；历史转出/死亡动物的旧记录仍允许加载；不重放状态变化。
 */
bool HealthManager::loadFromFile(const std::string &file) {
    std::vector<std::unique_ptr<MedicalRecord>> loaded;
    std::set<std::string> ids;
    /**
     * @brief 本行回调：验证6字段并恢复医疗派生对象，不重放状态。
     * @param f 依次为记录编号、动物编号、日期、兽医、类型、描述。
     * @return 合法并加入临时列表true，否则false。
     */
    bool ok = zoo_detail::readRows(file, lastError, [&](const auto &f) {
        if (f.size() != 6 || !animals.animalExists(f[1]) || !ids.insert(f[0]).second)
            return false;
        std::unique_ptr<MedicalRecord> r;
        if (f[4] == "CHECK")
            r = std::make_unique<HealthCheckRecord>(f[0], f[1], f[2], f[3], f[5]);
        else if (f[4] == "DISEASE")
            r = std::make_unique<DiseaseRecord>(f[0], f[1], f[2], f[3], f[5]);
        else if (f[4] == "TREATMENT")
            r = std::make_unique<TreatmentRecord>(f[0], f[1], f[2], f[3], f[5]);
        else if (f[4] == "RECOVERY")
            r = std::make_unique<RecoveryRecord>(f[0], f[1], f[2], f[3], f[5]);
        else
            return false;
        loaded.push_back(std::move(r));
        return true;
    });
    // 读取历史只恢复记录，绝不能重放疾病/康复并覆盖当前动物状态。
    if (ok)
        records.swap(loaded);
    return ok;
}
/**
 * @brief 以固定6字段格式保存全部医疗记录。
 * @param file 目标路径，父目录需存在。
 * @return 打开/写入/关闭均成功true，否则false。
 * @note 此方法直接覆盖单文件；菜单使用AStorage协调三文件。
 */
bool HealthManager::saveToFile(const std::string &file) const {
    std::ostringstream s;
    for (const auto &r : records) {
        auto i = r->toInfo();
        s << i.recordId << '|' << i.animalId << '|' << i.date << '|' << i.veterinarianId << '|'
          << i.type << '|' << i.description << '\n';
    }
    return zoo_detail::writeText(file, s.str(), lastError);
}
