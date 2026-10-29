#pragma once
#include "HealthManager.h"
#include <utility>
// A只保存自己的三个文件；B/C文件由对应子系统保存。
/**
 * @brief 协调A子系统园区、动物和医疗三个文件的启动加载与保存。
 * @note 三个Manager和数据目录应由本进程独占；不负责B/C文件；不提供断电级数据库事务。
 */
class AStorage {
  public:
    /**
     * @brief 构造AStorage并绑定协作对象。
     * @param z [in,out] 园区管理器引用。
     * @param a [in,out] 同一份动物管理器引用。
     * @param h [in,out] 健康管理器引用。
     * @param directory 三种A数据文件所在目录路径。
     * @note 构造函数无返回值；被引用对象必须覆盖本对象的整个使用期间。
     */
    AStorage(ZoneManager &z, AnimalManager &a, HealthManager &h, std::string directory)
        : zones(z), animals(a), health(h), directory(std::move(directory)) {}
    /**
     * @brief 启动时按园区→动物→医疗顺序验证并加载A的数据。
     * 参数：无。
     * @return 全部加载成功true，否则false并记录错误。
     * @note 先用临时对象验证；随后正式读取，期间需独占目录。第二次读取若发生I/O故障可能部分更新内存，调用方必须终止启动，不能继续保存。
     */
    bool load();
    /**
     * @brief 先写临时目录，再备份并提交A的三个数据文件。
     * 参数：无。
     * @return 完整提交成功true；普通I/O失败false并尝试回滚旧磁盘版本。
     * @note 失败时内存修改不撤销；只清理自己创建的临时目录；断电不保证跨文件原子性。
     */
    bool save();
    /**
     * @brief 读取最近一次加载/保存失败的原因。
     * 参数：无。
     * @return 内部错误说明的只读引用。
     * @note 后续load/save会更新；成功时清空。
     */
    const std::string &getLastError() const {
        return lastError;
    }

  private:
    ZoneManager &zones;
    AnimalManager &animals;
    HealthManager &health;
    std::string directory, lastError;
};
