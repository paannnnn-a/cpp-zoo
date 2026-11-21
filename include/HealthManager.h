#pragma once
#include "AnimalManager.h"
#include "DiseaseRecord.h"
#include "HealthCheckRecord.h"
#include "RecoveryRecord.h"
#include "TreatmentRecord.h"
#include <memory>
#include <optional>
#include <vector>
/**
 * @brief 管理多态医疗记录，并通过AnimalManager完成经确认的状态联动。
 * @note 动物管理器必须比本对象活得更久；记录变更仅在内存中生效，菜单再调用AStorage保存。
 */
class HealthManager {
  public:
    /**
     * @brief 构造健康管理器并绑定共享动物数据。
     * @param a [in,out] 可修改状态的AnimalManager引用，不复制动物。
     * @note 不自动加载历史。
     */
    explicit HealthManager(AnimalManager &a) : animals(a) {}
    /**
     * @brief 查找第一个未使用的M前缀医疗编号。
     * @return 例如M001；超过三位时自然扩展。
     */
    std::string generateRecordId() const;
    /**
     * @brief 复制并登记体检记录，保持动物当前状态。
     * @param r 已通过构造校验的体检记录；Manager再检查编号和动物。
     * @return 登记成功true，否则false。
     * @note 只登记在园动物；记录副本由本Manager管理。
     */
    bool addHealthCheck(const HealthCheckRecord &r);
    // 不传状态时仅登记；传入时只允许 TREATMENT/ISOLATED。
    /**
     * @brief 登记疾病，可选择同时改为治疗中或隔离。
     * @param r 待复制的疾病记录。
     * @param status 可选目标状态；nullopt只记录；仅接受TREATMENT/ISOLATED。
     * @return 记录与可选状态均成功true，否则false。
     * @note 不允许将不存在、转出或死亡动物登记为新的在园医疗行为。
     */
    bool addDiseaseRecord(const DiseaseRecord &r,
                          std::optional<AnimalStatus> status = std::nullopt);
    /**
     * @brief 登记治疗方案记录，不自动改变当前状态。
     * @param r 待复制的治疗记录。
     * @return 成功true；动物或编号不合法false。
     */
    bool addTreatmentRecord(const TreatmentRecord &r);
    /**
     * @brief 登记康复记录，并按选择恢复正常展出状态。
     * @param r 待复制的康复记录。
     * @param restoreNormal true恢复NORMAL；false只记录，默认true。
     * @return 记录及可选状态更新均成功true，否则false。
     * @note 转出或死亡动物不能通过此接口重新变成在园正常状态。
     */
    bool addRecoveryRecord(const RecoveryRecord &r, bool restoreNormal = true);
    /**
     * @brief 查询某动物全部医疗历史，并按日期稳定排序。
     * @param id 待查询动物编号。
     * @return 医疗记录副本列表；无记录/未知动物为空；同日期保持登记先后顺序。
     */
    std::vector<MedicalRecordInfo> getMedicalRecordsByAnimal(const std::string &id) const;
    /**
     * @brief 从6字段医疗文件加载历史，成功后整体替换医疗列表。
     * @param file UTF-8医疗文件路径；须先加载动物。
     * @return 全部记录合法true；文件/字段/关联错误false。
     * @note 失败保留旧记录；历史转出/死亡动物的旧记录仍允许加载；不重放状态变化。
     */
    bool loadFromFile(const std::string &file);
    /**
     * @brief 以固定6字段格式保存全部医疗记录。
     * @param file 目标路径，父目录需存在。
     * @return 打开/写入/关闭均成功true，否则false。
     * @note 此方法直接覆盖单文件；菜单使用AStorage协调三文件。
     */
    bool saveToFile(const std::string &file) const;
    /**
     * @brief 读取最近一次医疗修改或文件操作的错误说明。
     * @return 内部错误字符串的只读引用。
     * @note 查询和编号生成不清空错误；后续修改/文件操作可能改变引用内容。
     */
    const std::string &getLastError() const {
        return lastError;
    }

  private:
    AnimalManager &animals;
    std::vector<std::unique_ptr<MedicalRecord>> records;
    mutable std::string lastError;
    /**
     * @brief 检查动物是否在园及记录编号是否重复，再向内存中的记录列表追加记录。
     * @param record [in] 待接管的非空医疗unique_ptr。
     * @param status 可选目标状态；调用者负责限定可用枚举。
     * @return 全部成功true；校验失败false；状态更新失败撤销本条记录。
     * @note 记录仅追加到内存；如需联动状态，更新失败时移除刚追加的记录。
     */
    bool append(std::unique_ptr<MedicalRecord> record,
                std::optional<AnimalStatus> status = std::nullopt);
};
