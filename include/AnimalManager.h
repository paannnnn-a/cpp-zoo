#ifndef ANIMALMANAGER_H
#define ANIMALMANAGER_H

#include <memory>
#include <string>
#include <vector>
#include "AnimalCommon.h"
#include "Animal.h"
#include "ZoneManager.h"

/*文件说明：管理接口，可供另外两个子系统使用*/

class AnimalManager {
public:
    explicit AnimalManager(const ZoneManager& zoneManager) : zones(zoneManager) {}
    ~AnimalManager() = default;
    AnimalManager(const AnimalManager&) = delete;
    AnimalManager& operator=(const AnimalManager&) = delete;
    AnimalManager(AnimalManager&&) = delete;
    AnimalManager& operator=(AnimalManager&&) = delete;

    // B/A：不存在返回 false；已转出/死亡的档案仍算存在。
    bool animalExists(const std::string& animalId) const;
    // B/C/A：成功返回副本；不存在返回 false，outInfo 保持原值。
    bool getAnimalBasicInfo(const std::string& animalId, AnimalBasicInfo& outInfo) const;
    // B：NORMAL/TREATMENT/ISOLATED 可喂养，其他状态及不存在返回 false。
    bool canReceiveFeeding(const std::string& animalId) const;
    // C：仅 NORMAL 展出，不存在返回 false。
    bool isAnimalOnDisplay(const std::string& animalId) const;
    // C/A：名称或物种包含关键字即匹配；空关键字约定返回全部档案。
    std::vector<AnimalBasicInfo> searchAnimals(const std::string& keyword) const;
    // C/A：返回园区内全部档案，由 onDisplay 字段说明是否展出。
    std::vector<AnimalBasicInfo> getAnimalsByZone(const std::string& zoneId) const;

private:
    // unique_ptr 自动释放各派生对象；不把 Animal* 暴露给 B/C。
    std::vector<std::unique_ptr<Animal>> animals;
    // zones 必须比此 Manager 活得更久；Main 先创建 zones 再创建 animals。
    const ZoneManager& zones;
    // CRUD、编号生成与查找辅助函数在阶段4补充。
};
#endif
