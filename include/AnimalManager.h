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
 * 功能：保存A子系统完整动物档案所需的查询结果。
 *
 * basic：原来的动物基本信息，包括编号、名称、物种、园区和状态。
 * age：动物年龄。
 * gender：动物性别。
 *
 * 说明：这是查询结果副本，修改它不会修改Manager内部的动物。
 *       不修改B/C已经使用的AnimalBasicInfo，避免影响组员接口。
 */
/**
 * @brief 无继承关系的查询值类型，无成员方法；各字段保存对应业务信息，修改副本不会修改管理器内的对象。
 */
struct AnimalDetailInfo {
    AnimalBasicInfo basic;
    int age = 0;
    std::string gender;
};

/**
 * @brief 无继承关系的动物管理器；animals 独占多态动物，zones 引用园区管理器，lastError 保存错误。方法提供登记、查询、修改及文件读写。
 */
class AnimalManager {
public:
    /**
     * @brief 通过初始化列表初始化成员或绑定协作对象；实体构造复用字段校验，派生动物委托 Animal 构造。
     * @param zoneManager 园区管理器引用，生命周期须覆盖本对象。
         */
    explicit AnimalManager(const ZoneManager& zoneManager) : zones(zoneManager) {}
    /**
     * @brief 使用默认析构释放成员资源；虚析构同时执行派生类析构。
     * @note 参数：无。
         */
    ~AnimalManager() = default;
    /**
     * @brief 通过初始化列表初始化成员或绑定协作对象；实体构造复用字段校验，派生动物委托 Animal 构造。
         */
    AnimalManager(const AnimalManager&) = delete;
    AnimalManager& operator=(const AnimalManager&) = delete;
    /**
     * @brief 通过初始化列表初始化成员或绑定协作对象；实体构造复用字段校验，派生动物委托 Animal 构造。
         */
    AnimalManager(AnimalManager&&) = delete;
    AnimalManager& operator=(AnimalManager&&) = delete;

    // 不存在返回 false；已转出/死亡的档案仍算存在。
    /**
     * @brief 调用内部编号查找，根据是否得到非空指针返回结果；转出和死亡档案仍存在。
     * @param animalId 动物唯一编号。
         */
    bool animalExists(const std::string& animalId) const;
    // 成功返回副本；不存在返回 false，outInfo 保持原值。
    /**
     * @brief 按编号查找内部对象，再调用 buildBasicInfo；查找失败返回 false。
     * @param animalId 动物唯一编号。
     * @param outInfo 成功时写入的查询副本；失败保持原值。
         */
    bool getAnimalBasicInfo(const std::string& animalId, AnimalBasicInfo& outInfo) const;
    /**
    * 功能：查询动物详细信息，供完整档案页面使用。
    *
    * 参数：
    * animalId：目标动物编号，例如P001。
    * out：[输出参数] 成功时写入基本信息、年龄和性别；
    *      普通查询失败时保持原值。
    *
    * 返回：
    * true：动物存在，且关联园区信息有效。
    * false：动物不存在或关联园区不存在。
    *
    * 说明：返回副本，不向外暴露Animal指针。
    * @details 查找动物并构造临时基本信息，再填入年龄、性别，最后提交输出副本。
 */
    bool getAnimalDetailInfo(
        const std::string& animalId,
        AnimalDetailInfo& out
    ) const;
    // NORMAL/TREATMENT/ISOLATED 可喂养，其他状态及不存在返回 false。
    /**
     * @brief 先检查动物存在，再按状态分支：正常、治疗中、隔离允许喂养，转出和死亡不允许。
     * @param animalId 动物唯一编号。
         */
    bool canReceiveFeeding(const std::string& animalId) const;
    // 仅 NORMAL 展出，不存在返回 false。
    /**
     * @brief 检查动物存在且状态等于 NORMAL，利用短路求值避免访问空指针。
     * @param animalId 动物唯一编号。
         */
    bool isAnimalOnDisplay(const std::string& animalId) const;
    // 名称或物种包含关键字即匹配；空关键字约定返回全部档案。
    /**
     * @brief 逐个检查名称或物种是否包含关键字，将成功构造的基本信息副本加入结果。
     * @param keyword 名称或物种的子串；空串匹配全部。
         */
    std::vector<AnimalBasicInfo> searchAnimals(const std::string& keyword) const;
    // 返回园区内全部档案，由 onDisplay 字段说明是否展出。
    /**
     * @brief 先确认园区存在，再遍历动物按园区编号筛选并返回基本信息副本。
     * @param zoneId 所属园区编号。
         */
    std::vector<AnimalBasicInfo> getAnimalsByZone(const std::string& zoneId) const;
    // 本子系统内部使用：接收一个已经创建好的动物对象。
    /**
     * @brief 校验对象、编号唯一性及园区关联；字段重载先按类别构造派生对象，再移交 unique_ptr 所有权。
     * @param animal 待处理动物对象；unique_ptr 参数转移所有权。
         */
    bool addAnimal(std::unique_ptr<Animal> animal);

    // 内部使用：根据字段创建动物，并自动生成编号。
    /**
     * @brief 校验对象、编号唯一性及园区关联；字段重载先按类别构造派生对象，再移交 unique_ptr 所有权。
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

    // 修改普通资料，不允许修改 AnimalID。
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

    // 单独修改状态，后续 HealthManager 也会调用。
    /**
     * @brief 先查找动物并校验枚举，再调用状态 Setter；失败返回 false。
     * @param animalId 动物唯一编号。
     * @param status 动物状态。
         */
    bool changeAnimalStatus(
        const std::string& animalId,
        AnimalStatus status
    );

    //从文件加载
    /**
     * @brief 读取并拆分文件字段，校验格式、编号唯一性及关联对象，先存入临时容器，全部成功后交换正式数据。
     * @param filename 读写文件路径。
         */
    bool loadFromFile(const std::string& filename);
    //写入文件
    /**
     * @brief 按固定字段顺序和竖线分隔符序列化记录，检查文件写入结果并返回是否成功。
     * @param filename 读写文件路径。
         */
    bool saveToFile(const std::string& filename) const;

    const std::string& getLastError() const;


private:
    // unique_ptr 自动释放各派生对象。
    std::vector<std::unique_ptr<Animal>> animals;
    // zones 必须比此 Manager 活得更久；main 先创建 zones 再创建 animals。
    const ZoneManager& zones;
    // 自动生成尚未使用的动物编号。
    /**
     * @brief 从 1 开始递增，格式化为 P 加至少三位数字，返回第一个未占用编号。
     * @note 参数：无。
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
    // 把内部 Animal 转换成对外返回的基本信息副本。
    /**
     * @brief 先查询关联园区，再复制动物字段并计算展出状态，全部完成后写入输出参数。
     * @param animal 待处理动物对象；unique_ptr 参数转移所有权。
     * @param outInfo 成功时写入的查询副本；失败保持原值。
         */
    bool buildBasicInfo(const Animal& animal,AnimalBasicInfo& outInfo) const;
    // const 保存函数也需要记录错误；mutable 允许修改错误信息，不允许借此修改动物数据。
    mutable std::string lastError;
};
#endif
