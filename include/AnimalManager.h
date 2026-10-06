#ifndef ANIMALMANAGER_H
#define ANIMALMANAGER_H

#include <memory>
#include <string>
#include <vector>
#include "AnimalCommon.h"
#include "Animal.h"
#include "ZoneManager.h"

/*文件说明：以下是动物管理接口，可供另外两个子系统使用*/

class AnimalManager {
public:
    explicit AnimalManager(const ZoneManager& zoneManager) : zones(zoneManager) {}
    ~AnimalManager() = default;
    AnimalManager(const AnimalManager&) = delete;
    AnimalManager& operator=(const AnimalManager&) = delete;
    AnimalManager(AnimalManager&&) = delete;
    AnimalManager& operator=(AnimalManager&&) = delete;

    // 不存在返回 false；已转出/死亡的档案仍算存在。
    bool animalExists(const std::string& animalId) const;
    // 成功返回副本；不存在返回 false，outInfo 保持原值。
    bool getAnimalBasicInfo(const std::string& animalId, AnimalBasicInfo& outInfo) const;
    // NORMAL/TREATMENT/ISOLATED 可喂养，其他状态及不存在返回 false。
    bool canReceiveFeeding(const std::string& animalId) const;
    // 仅 NORMAL 展出，不存在返回 false。
    bool isAnimalOnDisplay(const std::string& animalId) const;
    // 名称或物种包含关键字即匹配；空关键字约定返回全部档案。
    std::vector<AnimalBasicInfo> searchAnimals(const std::string& keyword) const;
    // 返回园区内全部档案，由 onDisplay 字段说明是否展出。
    std::vector<AnimalBasicInfo> getAnimalsByZone(const std::string& zoneId) const;
    // 本子系统内部使用：接收一个已经创建好的动物对象。
    bool addAnimal(std::unique_ptr<Animal> animal);

    // 内部使用：根据字段创建动物，并自动生成编号。
    bool addAnimal(
        const std::string& category,
        const std::string& name,
        const std::string& species,
        int age,
        const std::string& gender,
        const std::string& zoneId
    );

    // 修改普通资料，不允许修改 AnimalID。
    bool updateAnimalInfo(
        const std::string& animalId,
        const std::string& name,
        int age,
        const std::string& gender,
        const std::string& zoneId
    );

    // 单独修改状态，后续 HealthManager 也会调用。
    bool changeAnimalStatus(
        const std::string& animalId,
        AnimalStatus status
    );

    //从文件加载
    bool loadFromFile(const std::string& filename);
    //写入文件
    bool saveToFile(const std::string& filename) const;

    const std::string& getLastError() const;


private:
    // unique_ptr 自动释放各派生对象。
    std::vector<std::unique_ptr<Animal>> animals;
    // zones 必须比此 Manager 活得更久；main 先创建 zones 再创建 animals。
    const ZoneManager& zones;
    // 自动生成尚未使用的动物编号。
    std::string generateAnimalId() const;
    // 内部使用，不把 Animal* 暴露给 B/C。
    Animal* findAnimalInternal(const std::string& animalId);
    const Animal* findAnimalInternal(const std::string& animalId) const;
    // 把内部 Animal 转换成对外返回的基本信息副本。
    bool buildBasicInfo(const Animal& animal,AnimalBasicInfo& outInfo) const;
    // const 保存函数也需要记录错误；mutable 允许修改错误信息，不允许借此修改动物数据。
    mutable std::string lastError;
};
#endif
