#include "AStorage.h"
#include <chrono>
#include <filesystem>
namespace fs = std::filesystem;
/**
 * @brief 启动时按园区→动物→医疗顺序验证并加载A的数据。
 * 参数：无。
 * @return 全部加载成功true，否则false并记录错误。
 * @note 先用临时对象验证；随后正式读取，期间需独占目录。第二次读取若发生I/O故障可能部分更新内存，调用方必须终止启动，不能继续保存。
 */
bool AStorage::load() {
    lastError.clear();
    // 先用临时对象验证完整数据集；预验证失败不会覆盖当前内存。
    ZoneManager z;
    AnimalManager a(z);
    HealthManager h(a);
    /**
     * @brief 构造数据目录内某一文件的路径。
     * @param n 不含目录的固定数据文件名。
     * @return 拼接后的路径字符串。
     */
    auto path = [&](const char *n) { return (fs::path(directory) / n).string(); };
    if (!z.loadFromFile(path("zones.txt"))) {
        lastError = z.getLastError();
        return false;
    }
    if (!a.loadFromFile(path("animals.txt"))) {
        lastError = a.getLastError();
        return false;
    }
    if (!h.loadFromFile(path("medical_records.txt"))) {
        lastError = h.getLastError();
        return false;
    }
    // 此接口用于启动加载，读取期间数据目录应由本进程独占。
    if (!zones.loadFromFile(path("zones.txt")) || !animals.loadFromFile(path("animals.txt")) ||
        !health.loadFromFile(path("medical_records.txt"))) {
        lastError = "正式加载失败，请停止启动并检查数据文件";
        return false;
    }
    return true;
}
/**
 * @brief 先写临时目录，再备份并提交A的三个数据文件。
 * 参数：无。
 * @return 完整提交成功true；普通I/O失败false并尝试回滚旧磁盘版本。
 * @note 失败时内存修改不撤销；只清理自己创建的临时目录；断电不保证跨文件原子性。
 */
bool AStorage::save() {
    lastError.clear();
    fs::path stage;
    bool ownsStage = false;
    const fs::path root(directory);
    const char *names[] = {"zones.txt", "animals.txt", "medical_records.txt"};
    bool existed[3] = {false, false, false};
    bool touched[3] = {false, false, false};
    try {
        const auto stamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        stage = root / (".a-save-" + std::to_string(stamp));
        if (!fs::create_directory(stage))
            throw std::runtime_error("无法创建临时保存目录");
        ownsStage = true;
        // 全部写入临时目录后才触碰真实文件，防止某个文件打不开时破坏原数据。
        if (!zones.saveToFile((stage / names[0]).string()))
            throw std::runtime_error(zones.getLastError());
        if (!animals.saveToFile((stage / names[1]).string()))
            throw std::runtime_error(animals.getLastError());
        if (!health.saveToFile((stage / names[2]).string()))
            throw std::runtime_error(health.getLastError());
        for (int i = 0; i < 3; ++i) {
            existed[i] = fs::exists(root / names[i]);
            if (existed[i]) {
                if (!fs::is_regular_file(root / names[i]))
                    throw std::runtime_error("保存目标不是普通文件");
                fs::copy_file(root / names[i], stage / (std::string("old_") + names[i]));
            }
        }
        for (int i = 0; i < 3; ++i) {
            touched[i] = true;
            if (existed[i])
                fs::remove(root / names[i]);
            fs::rename(stage / names[i], root / names[i]);
        }
        std::error_code ignored;
        fs::remove_all(stage, ignored);
        return true;
    } catch (const std::exception &e) {
        lastError = e.what();
        bool restored = true;
        // 普通I/O异常尽力还原旧版本；断电/强制结束不属于此回滚保证。
        for (int i = 0; i < 3; ++i)
            if (touched[i]) {
                std::error_code ec;
                if (existed[i])
                    fs::copy_file(stage / (std::string("old_") + names[i]), root / names[i],
                                  fs::copy_options::overwrite_existing, ec);
                else
                    fs::remove(root / names[i], ec);
                if (ec)
                    restored = false;
            }
        if (ownsStage) {
            if (restored) {
                std::error_code ignored;
                fs::remove_all(stage, ignored);
            } else
                lastError += "；回滚失败，备份保留于 " + stage.string();
        }
        return false;
    }
}
