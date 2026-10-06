#include "AnimalManager.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "Mammal.h"
#include "Bird.h"
#include "Reptile.h"
#include "detail/FieldValidation.h"

#include <charconv>
#include <fstream>
#include <set>
#include <system_error>

//非const版本;根据动物编号在内部容器里查找对应的动物，返回可修改的指针，没有就返回nullptr。
Animal* AnimalManager::findAnimalInternal(const std::string& animalId) {
    for (const auto& animal : animals) {
        if (animal->getId() == animalId) {
            return animal.get();//unique_ptr里自带的函数。
        }
    }
    return nullptr;
}

//const版本;根据动物编号在内部容器里查找对应的动物，返回只读的指针。
const Animal* AnimalManager::findAnimalInternal(const std::string& animalId) const {
    for (const auto& animal : animals) {
        if (animal->getId() == animalId) {
            return animal.get();
        }
    }
    return nullptr;
}

//自动生成一个尚未使用过的动物编号：从 P001 开始，生成编号，利用findAnimalInternal()寻找第一个没有使用过的编号。
std::string AnimalManager::generateAnimalId() const {
    for (std::size_t number = 1; ; ++number) {
        std::ostringstream output;
        output << 'P'<< std::setfill('0')<< std::setw(3)<< number;
        const std::string candidate = output.str();
        if (findAnimalInternal(candidate) == nullptr) {
            return candidate;
        }
    }
}

//添加一个已经创建好的动物到管理器，参数表示把该动物的“管理权传进来；参数：创建好的动物智能指针。
bool AnimalManager::addAnimal(std::unique_ptr<Animal> animal) {
    // 防止调用方传入空智能指针。
    if (!animal) {
        return false;
    }
    // 动物编号不能重复。
    if (findAnimalInternal(animal->getId()) != nullptr) {
        return false;
    }
    // Animal 只检查园区编号格式；Manager 负责检查这个园区是否真的存在。
    if (!zones.zoneExists(animal->getZoneId())) {
        return false;
    }
    // 把对象所有权转交给动物容器。
    animals.push_back(std::move(animal));

    return true;
}

//根据类别和各种信息创建一只新动物，并加入管理器。
bool AnimalManager::addAnimal(
    const std::string& category,
    const std::string& name,
    const std::string& species,
    int age,
    const std::string& gender,
    const std::string& zoneId
) {
    if (!zones.zoneExists(zoneId)) {
        return false;
    }

    // category 必须使用统一类别名称。
    if (category != "Mammal" &&
        category != "Bird" &&
        category != "Reptile") {
        return false;
    }

    const std::string animalId = generateAnimalId();

    try {
        std::unique_ptr<Animal> animal;

        if (category == "Mammal") {
            animal = std::make_unique<Mammal>(animalId, name, species, age, gender, zoneId);
        }
        else if (category == "Bird") {
            animal = std::make_unique<Bird>(animalId, name, species, age, gender, zoneId);
        }
        else {
            animal = std::make_unique<Reptile>(animalId, name, species, age, gender, zoneId);
        }

        // 复用另一个重载，统一执行添加检查。
        return addAnimal(std::move(animal));

    }
    catch (const std::invalid_argument&) {
        // 构造函数发现非法字段。
        return false;
    }
}

//改变动物状态；参数：ID、新状态。
bool AnimalManager::changeAnimalStatus(const std::string& animalId,AnimalStatus status) {
    Animal* animal = findAnimalInternal(animalId);

    if (animal == nullptr) {
        return false;
    }

    if (!zoo_detail::validStatus(status)) {
        return false;
    }

    animal->setStatus(status);

    return true;
}

//把一个 Animal 对象转换成对外使用的 AnimalBasicInfo 结构体；参数：动物的引用，查询结果副本的引用（AnimalBasicInfo结构体）。
bool AnimalManager::buildBasicInfo(const Animal& animal,AnimalBasicInfo& outInfo) const {
    ZoneInfo zoneInfo;

    // 正常情况下，添加/加载动物时已保证园区存在，这里再检查一次，避免返回不完整的基本信息。
    if (!zones.getZoneInfo(animal.getZoneId(), zoneInfo)) {
        return false;
    }

    AnimalBasicInfo info;

    info.animalId = animal.getId();
    info.name = animal.getName();
    info.category = animal.getCategory();
    info.species = animal.getSpecies();

    info.zoneId = animal.getZoneId();
    info.zoneName = zoneInfo.zoneName;

    info.status = animal.getStatus();
    info.onDisplay = isAnimalOnDisplay(animal.getId());

    // 所有信息准备完成后，再把临时对象移到outInfo。
    outInfo = std::move(info);

    return true;
}

//判断动物是否存在；参数：ID。
bool AnimalManager::animalExists(const std::string& animalId) const {
    return findAnimalInternal(animalId) != nullptr;
}

//获取动物的基本信息；参数：动物的ID，查询结果副本的引用（AnimalBasicInfo结构体）
bool AnimalManager::getAnimalBasicInfo(const std::string& animalId,AnimalBasicInfo& outInfo) const {
    const Animal* animal = findAnimalInternal(animalId);

    if (animal == nullptr) {
        return false;
    }

    return buildBasicInfo(*animal, outInfo);
}

//判断指定ID的动物是否可以接受投喂；参数：动物ID。
bool AnimalManager::canReceiveFeeding(const std::string& animalId) const {
    const Animal* animal = findAnimalInternal(animalId);

    if (animal == nullptr) {
        return false;
    }

    switch (animal->getStatus()) {
    case AnimalStatus::NORMAL:
    case AnimalStatus::TREATMENT:
    case AnimalStatus::ISOLATED:
        return true;

    case AnimalStatus::TRANSFERRED:
    case AnimalStatus::DECEASED:
        return false;
    }

    return false;
}

//判断指定ID动物是否处于“展出”状态。参数：动物ID。
bool AnimalManager::isAnimalOnDisplay(const std::string& animalId) const {
    const Animal* animal = findAnimalInternal(animalId);

    return animal != nullptr && animal->getStatus() == AnimalStatus::NORMAL;
}

//按关键字模糊搜索动物（匹配名称或物种）；参数：关键词。
std::vector<AnimalBasicInfo> AnimalManager::searchAnimals(const std::string& keyword) const {
    std::vector<AnimalBasicInfo> result;

    for (const auto& animal : animals) {
        const bool matched =(animal->getName().find(keyword) != std::string::npos) ||(animal->getSpecies().find(keyword) != std::string::npos);

        if (!matched) {
            continue;
        }

        AnimalBasicInfo info;

        if (buildBasicInfo(*animal, info)) {
            result.push_back(std::move(info));
        }
    }

    return result;
}

//获取指定园区下的所有动物基本信息。参数：园区名称。
std::vector<AnimalBasicInfo> AnimalManager::getAnimalsByZone(const std::string& zoneId) const {
    std::vector<AnimalBasicInfo> result;

    if (!zones.zoneExists(zoneId)) {
        return result;
    }

    for (const auto& animal : animals) {
        if (animal->getZoneId() != zoneId) {
            continue;
        }

        AnimalBasicInfo info;

        if (buildBasicInfo(*animal, info)) {
            result.push_back(std::move(info));
        }
    }

    return result;
}

//错误信息获取
const std::string& AnimalManager::getLastError() const {
    return lastError;
}

//实现保存动物；参数：文件名。（但是可能出现只有半个文件的情况，还在想）
bool AnimalManager::saveToFile(const std::string& filename) const {
    lastError.clear();

    std::ofstream output(
        filename,
        std::ios::binary | std::ios::trunc
    );

    if (!output) {
        lastError = "无法写入动物文件：" + filename;
        return false;
    }

    for (const auto& animal : animals) {
        const std::string statusCode =
            animalStatusToCode(animal->getStatus());

        if (statusCode.empty()) {
            lastError = "动物状态非法：" + animal->getId();
            return false;
        }

        output << animal->getId() << '|'
            << animal->getName() << '|'
            << animal->getCategory() << '|'
            << animal->getSpecies() << '|'
            << animal->getAge() << '|'
            << animal->getGender() << '|'
            << animal->getZoneId() << '|'
            << statusCode << '\n';
    }

    output.close();

    // 检查最终写入结果，不能只检查文件是否成功打开。
    if (!output) {
        lastError = "动物文件写入失败：" + filename;
        return false;
    }

    return true;
}

//实现加载动物；参数：文件名。
bool AnimalManager::loadFromFile(const std::string& filename) {
    lastError.clear();

    std::ifstream input(filename, std::ios::binary);

    if (!input) {
        lastError = "无法打开动物文件：" + filename;
        return false;
    }

    // 先加载到临时容器，全部成功后才替换正式数据。
    std::vector<std::unique_ptr<Animal>> loaded;
    std::set<std::string> loadedIds;

    std::string line;
    std::size_t lineNumber = 0;

    while (std::getline(input, line)) {
        ++lineNumber;

        // 兼容 Windows 编辑器写出的 UTF-8 BOM。
        if (lineNumber == 1 &&
            line.compare(0, 3, "\xEF\xBB\xBF") == 0) {
            line.erase(0, 3);
        }

        // getline 去掉了 \n，但 Windows 文件可能还留下 \r。
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line.empty()) {
            continue;
        }

        // 本行所有错误统一附上行号。
        const auto fail = [&](const std::string& message) {
            lastError ="第 " + std::to_string(lineNumber) +" 行：" + message;
            return false;
        };

        // 按 | 拆分；保留末尾空字段，便于准确检查格式。
        std::vector<std::string> fields;
        std::size_t begin = 0;

        while (true) {
            const auto separator = line.find('|', begin);

            if (separator == std::string::npos) {
                fields.push_back(line.substr(begin));
                break;
            }

            fields.push_back(line.substr(begin, separator - begin));

            begin = separator + 1;
        }

        if (fields.size() != 8) {
            return fail("必须恰好有8个字段");
        }

        const std::string& id = fields[0];
        const std::string& name = fields[1];
        const std::string& category = fields[2];
        const std::string& species = fields[3];
        const std::string& ageText = fields[4];
        const std::string& gender = fields[5];
        const std::string& zoneId = fields[6];
        const std::string& statusText = fields[7];

        if (!zoo_detail::validId(id)) {
            return fail("AnimalID 格式非法");
        }

        if (!loadedIds.insert(id).second) {
            return fail("AnimalID 重复：" + id);
        }

        if (!zoo_detail::validText(name, true) ||
            !zoo_detail::validText(species, true) ||
            !zoo_detail::validText(gender, true)) {
            return fail("名称、物种或性别字段非法");
        }

        if (!zoo_detail::validId(zoneId) ||
            !zones.zoneExists(zoneId)) {
            return fail("园区编号非法或园区不存在：" + zoneId);
        }

        // from_chars 不抛异常，且可以检查是否解析了全部字符。
        int age = 0;

        const char* ageBegin = ageText.data();
        const char* ageEnd = ageBegin + ageText.size();

        const auto parsedAge = std::from_chars(ageBegin, ageEnd, age); //应该解析出std::from_chars_result结构体

        if (ageText.empty() ||
            parsedAge.ec != std::errc{} ||  //检查有没有“abc”这种解析不正常的。
            parsedAge.ptr != ageEnd ||  //检查有没有把整个字符都解析完，"123abc"。
            age < 0 ||
            ageText.front() == '-') {
            return fail("年龄必须是非负整数");
        }

        AnimalStatus status;

        if (!animalStatusFromCode(statusText, status)) {
            return fail("动物状态编码非法：" + statusText);
        }

        try {
            std::unique_ptr<Animal> animal;

            if (category == "Mammal") {
                animal = std::make_unique<Mammal>(
                    id, name, species, age, gender, zoneId, status
                );
            }
            else if (category == "Bird") {
                animal = std::make_unique<Bird>(
                    id, name, species, age, gender, zoneId, status
                );
            }
            else if (category == "Reptile") {
                animal = std::make_unique<Reptile>(
                    id, name, species, age, gender, zoneId, status
                );
            }
            else {
                return fail("动物类别非法：" + category);
            }

            loaded.push_back(std::move(animal));

        }
        catch (const std::invalid_argument& error) {
            return fail(error.what());
        }
    }

    if (input.bad() || (input.fail() && !input.eof())) {
        lastError = "动物文件读取失败：" + filename;
        return false;
    }

    // 所有记录都通过校验后，一次性替换正式列表。
    animals.swap(loaded);

    return true;
}