#ifndef ANIMALMANAGER_H
#define ANIMALMANAGER_H

#include <memory>
#include <string>
#include <vector>
#include "AnimalCommon.h"
#include "Animal.h"
#include "ZoneManager.h"

/*文件说明：以下是动物管理接口，可供另外两个子系统使用*/

/**
 * @brief 完整动物档案的查询副本，在基本信息之外包含年龄和性别。
 */
struct AnimalDetailInfo {
    AnimalBasicInfo basic;
    int age = 0;
    std::string gender;
};

/**
 * @brief 管理动物档案，负责登记、查询、修改和文件读写。animals 持有动物，zones 引用园区管理器。
 */
class AnimalManager {
public:
    /**
     * @brief 绑定园区管理器，供登记动物和查询园区信息时使用。
     * @param zoneManager 园区管理器引用，生命周期须覆盖本对象。
     */
    explicit AnimalManager(const ZoneManager& zoneManager) : zones(zoneManager) {}
    /// @brief 默认析构时，animals 中的每个 unique_ptr 自动释放它持有的动物。
    ~AnimalManager() = default;
    // 禁止拷贝构造和拷贝赋值。
    AnimalManager(const AnimalManager&) = delete;
    AnimalManager& operator=(const AnimalManager&) = delete;
    // 禁止移动构造和移动赋值。
    AnimalManager(AnimalManager&&) = delete;
    AnimalManager& operator=(AnimalManager&&) = delete;

    /**
     * @brief 按编号判断档案是否存在，包括已转出和已死亡的动物。
     * @param animalId 动物唯一编号。
     */
    bool animalExists(const std::string& animalId) const;
    /**
     * @brief 查询指定动物的基本信息；动物或关联园区不存在时返回 false。
     * @param animalId 动物唯一编号。
     * @param outInfo 成功时写入的查询副本；失败保持原值。
     */
    bool getAnimalBasicInfo(const std::string& animalId, AnimalBasicInfo& outInfo) const;
    /**
     * @brief 按编号查询动物的基本信息、年龄和性别。
     * @param animalId 目标动物编号。
     * @param[out] out 查询成功后写入的完整档案副本。
     * @return 动物或所在园区不存在时返回 false，保留 out 的原值。
     */
    bool getAnimalDetailInfo(
        const std::string& animalId,
        AnimalDetailInfo& out
    ) const;
    /**
     * @brief 先检查动物存在，再按状态分支：正常、治疗中、隔离允许喂养，转出和死亡不允许。
     * @param animalId 动物唯一编号。
     */
    bool canReceiveFeeding(const std::string& animalId) const;
    /**
     * @brief 仅当动物存在且状态为 NORMAL 时展出。
     * @param animalId 动物唯一编号。
     */
    bool isAnimalOnDisplay(const std::string& animalId) const;
    /**
     * @brief 逐个检查名称或物种是否包含关键字，将成功构造的基本信息副本加入结果。
     * @param keyword 名称或物种的子串；空串匹配全部。
     */
    std::vector<AnimalBasicInfo> searchAnimals(const std::string& keyword) const;
    // 返回园区内全部档案，由 onDisplay 字段说明是否展出。
    /**
     * @brief 先确认园区存在，再遍历动物按园区编号筛选并返回基本信息副本。
     * @param zoneId 要查询的园区编号，例如 Z001。
     */
    std::vector<AnimalBasicInfo> getAnimalsByZone(const std::string& zoneId) const;
    /**
     * @brief 接收动物对象，通过编号和园区检查后将其存入 animals。
     * @param animal 按值接收所有权。校验失败时返回 false，传入的对象随参数销毁。
     */
    bool addAnimal(std::unique_ptr<Animal> animal);

    /**
     * @brief 生成动物编号，按类别构造派生对象并交给对象重载登记。
     * @param category Mammal、Bird 或 Reptile 类别编码。
     * @param name 名称。
     * @param species 物种。
     * @param age 非负年龄。
     * @param gender 性别文本。
     * @param zoneId 所属园区编号。
     */
    bool addAnimal(
        const std::string& category,
        const std::string& name,
        const std::string& species,
        int age,
        const std::string& gender,
        const std::string& zoneId
    );

    /**
     * @brief 修改指定编号的普通资料，保留动物唯一编号；调用方应检查布尔结果。
     * @param animalId 动物唯一编号。
     * @param name 名称。
     * @param age 非负年龄。
     * @param gender 性别文本。
     * @param zoneId 所属园区编号。
     */
    bool updateAnimalInfo(
        const std::string& animalId,
        const std::string& name,
        int age,
        const std::string& gender,
        const std::string& zoneId
    );

    // HealthManager 登记疾病或康复记录时也通过此接口更新状态。
    /**
     * @brief 先查找动物并校验枚举，再调用状态 Setter；失败返回 false。
     * @param animalId 动物唯一编号。
     * @param status 动物状态。
     */
    bool changeAnimalStatus(
        const std::string& animalId,
        AnimalStatus status
    );

    /**
     * @brief 读取并拆分文件字段，校验格式、编号唯一性及关联对象，先存入临时容器，全部成功后交换正式数据。
     * @param filename 读写文件路径。
     */
    bool loadFromFile(const std::string& filename);
    /**
     * @brief 覆盖写入动物文件，关闭文件后检查写入结果。
     * @param filename 读写文件路径。
     */
    bool saveToFile(const std::string& filename) const;

    /// @brief 返回最近一次增改动物或读写文件的错误说明；这些操作成功时清空。
    const std::string& getLastError() const;


private:
    // unique_ptr 自动释放各派生对象。
    std::vector<std::unique_ptr<Animal>> animals;
    // zones 必须比此 Manager 活得更久；main 先创建 zones 再创建 animals。
    const ZoneManager& zones;
    /**
     * @brief 从 1 开始递增，格式化为 P 加至少三位数字，返回第一个未占用编号。
     */
    std::string generateAnimalId() const;
    // 内部使用，不把 Animal* 暴露给 B/C。
    /**
     * @brief 遍历 animals 并比较编号，命中返回内部指针，未命中返回 nullptr；不转移所有权。
     * @param animalId 动物唯一编号。
     */
    Animal* findAnimalInternal(const std::string& animalId);
    /**
     * @brief 遍历 animals 并比较编号，命中返回内部指针，未命中返回 nullptr；不转移所有权。
     * @param animalId 动物唯一编号。
     */
    const Animal* findAnimalInternal(const std::string& animalId) const;
    /**
     * @brief 先查询关联园区，再复制动物字段并计算展出状态，全部完成后写入输出参数。
     * @param animal 以 const Animal& 只读访问的动物，所有权仍属于原持有者。
     * @param outInfo 成功时写入的查询副本；失败保持原值。
     */
    bool buildBasicInfo(const Animal& animal,AnimalBasicInfo& outInfo) const;
    // const 保存函数也需要记录错误；mutable 允许修改错误信息，不允许借此修改动物数据。
    mutable std::string lastError;
};
#endif
