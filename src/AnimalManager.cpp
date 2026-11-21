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

/**
 * @brief 遍历 animals 并比较编号，命中返回内部指针，未命中返回 nullptr；不转移所有权。
 * @param animalId 动物唯一编号。
 */
Animal* AnimalManager::findAnimalInternal(const std::string& animalId) {
    for (const auto& animal : animals) {
        if (animal->getId() == animalId) {
            return animal.get();
        }
    }
    return nullptr;
}

/**
 * @brief 遍历 animals 并比较编号，命中返回内部指针，未命中返回 nullptr；不转移所有权。
 * @param animalId 动物唯一编号。
 */
const Animal* AnimalManager::findAnimalInternal(const std::string& animalId) const {
    for (const auto& animal : animals) {
        if (animal->getId() == animalId) {
            return animal.get();
        }
    }
    return nullptr;
}

/**
 * @brief 从 1 开始递增，格式化为 P 加至少三位数字，返回第一个未占用编号。
 */
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

/**
 * @brief 接收动物对象，通过编号和园区检查后将其存入 animals。
 * @param animal 按值接收所有权。校验失败时返回 false，传入的对象随参数销毁。
 */
bool AnimalManager::addAnimal(std::unique_ptr<Animal> animal) {
    // 防止调用方传入空智能指针。
    if (!animal) {
        lastError = "动物对象为空";
        return false;
    }
    // 动物编号不能重复。
    if (findAnimalInternal(animal->getId()) != nullptr) {
        lastError = "动物编号重复：" + animal->getId();
        return false;
    }
    // Animal 只检查园区编号格式；Manager 负责检查这个园区是否真的存在。
    if (!zones.zoneExists(animal->getZoneId())) {
        lastError = "园区不存在：" + animal->getZoneId();
        return false;
    }
    // 把对象所有权转交给动物容器。
    animals.push_back(std::move(animal));
    lastError.clear();

    return true;
}

/**
 * @brief 生成动物编号，按类别构造派生对象并交给对象重载登记。
 * @param category Mammal、Bird 或 Reptile 类别编码。
 * @param name 名称。
 * @param species 物种。
 * @param age 非负年龄。
 * @param gender 性别文本。
 * @param zoneId 所属园区编号。
 */
bool AnimalManager::addAnimal(
    const std::string& category,
    const std::string& name,
    const std::string& species,
    int age,
    const std::string& gender,
    const std::string& zoneId
) {
    if (!zones.zoneExists(zoneId)) {
        lastError = "园区不存在：" + zoneId;
        return false;
    }

    // category 必须使用统一类别名称。
    if (category != "Mammal" &&
        category != "Bird" &&
        category != "Reptile") {
        lastError = "动物类别无效：" + category;
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
    catch (const std::invalid_argument& error) {
        lastError = error.what();
        return false;
    }
}

/**
 * @brief 先查找动物并校验枚举，再调用状态 Setter；失败返回 false。
 * @param animalId 动物唯一编号。
 * @param status 动物状态。
 */
bool AnimalManager::changeAnimalStatus(const std::string& animalId,AnimalStatus status) {
    Animal* animal = findAnimalInternal(animalId);

    if (animal == nullptr) {
        lastError = "动物不存在：" + animalId;
        return false;
    }

    if (!zoo_detail::validStatus(status)) {
        lastError = "动物状态无效";
        return false;
    }

    animal->setStatus(status);
    lastError.clear();

    return true;
}

/**
 * @brief 先查询关联园区，再复制动物字段并计算展出状态，全部完成后写入输出参数。
 * @param animal 以 const Animal& 只读访问的动物，所有权仍属于原持有者。
 * @param outInfo 成功时写入的查询副本；失败保持原值。
 */
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

/**
 * @brief 调用内部编号查找，根据是否得到非空指针返回结果；转出和死亡档案仍存在。
 * @param animalId 动物唯一编号。
 */
bool AnimalManager::animalExists(const std::string& animalId) const {
    return findAnimalInternal(animalId) != nullptr;
}

/**
 * @brief 按编号查找内部对象，再调用 buildBasicInfo；查找失败返回 false。
 * @param animalId 动物唯一编号。
 * @param outInfo 成功时写入的查询副本；失败保持原值。
 */
bool AnimalManager::getAnimalBasicInfo(const std::string& animalId,AnimalBasicInfo& outInfo) const {
    const Animal* animal = findAnimalInternal(animalId);

    if (animal == nullptr) {
        return false;
    }

    return buildBasicInfo(*animal, outInfo);
}

/**
 * @brief 按编号查询动物的基本信息、年龄和性别。
 * @param animalId 目标动物编号。
 * @param[out] out 查询成功后写入的完整档案副本。
 * @return 动物或所在园区不存在时返回 false，保留 out 的原值。
 */
bool AnimalManager::getAnimalDetailInfo(
    const std::string& animalId,
    AnimalDetailInfo& out
) const {
    const Animal* animal = findAnimalInternal(animalId);

    if (animal == nullptr) {
        return false;
    }

    AnimalDetailInfo info;

    // 先取得包含园区和展出状态的基本信息，再补充年龄和性别。
    if (!buildBasicInfo(*animal, info.basic)) {
        return false;
    }

    info.age = animal->getAge();
    info.gender = animal->getGender();

    // 完整结果准备好后，再交给调用方。
    out = std::move(info);

    return true;
}

/**
 * @brief 先检查动物存在，再按状态分支：正常、治疗中、隔离允许喂养，转出和死亡不允许。
 * @param animalId 动物唯一编号。
 */
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

/**
 * @brief 检查动物存在且状态等于 NORMAL，利用短路求值避免访问空指针。
 * @param animalId 动物唯一编号。
 */
bool AnimalManager::isAnimalOnDisplay(const std::string& animalId) const {
    const Animal* animal = findAnimalInternal(animalId);

    return animal != nullptr && animal->getStatus() == AnimalStatus::NORMAL;
}

/**
 * @brief 逐个检查名称或物种是否包含关键字，将成功构造的基本信息副本加入结果。
 * @param keyword 名称或物种的子串；空串匹配全部。
 */
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

/**
 * @brief 先确认园区存在，再遍历动物按园区编号筛选并返回基本信息副本。
 * @param zoneId 要查询的园区编号，例如 Z001。
 */
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

/**
 * @brief 覆盖写入动物文件，关闭文件后检查写入结果。
 * @param filename 读写文件路径。
 */
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

/**
 * @brief 读取并拆分文件字段，校验格式、编号唯一性及关联对象，先存入临时容器，全部成功后交换正式数据。
 * @param filename 读写文件路径。
 */
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

        const auto parsedAge = std::from_chars(ageBegin, ageEnd, age);

        if (ageText.empty() ||
            parsedAge.ec != std::errc{} ||  // 拒绝无法解析或溢出的数值。
            parsedAge.ptr != ageEnd ||  // 拒绝数字后的多余字符。
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
/**
 * @brief 修改动物普通资料，保持编号、物种、类别及状态不变。
 * @param animalId 目标动物编号。
 * @param name 新名称。
 * @param age 非负年龄。
 * @param gender 新性别文本。
 * @param zoneId 已存在的目标园区编号。
 * @return 成功返回 true；业务校验失败返回 false 并保留原资料。
 * @details 在任何 Setter 调用前校验全部输入，避免非法字段导致部分修改。
 */
bool AnimalManager::updateAnimalInfo(const std::string& animalId,
                                    const std::string& name, int age,
                                    const std::string& gender,
                                    const std::string& zoneId) {
    lastError.clear();
    Animal* animal = findAnimalInternal(animalId);
    if (!animal) {
        lastError = "Animal not found: " + animalId;
        return false;
    }
    if (!zoo_detail::validText(name) || !zoo_detail::validText(gender) || age < 0 ||
        !zoo_detail::validId(zoneId) || !zones.zoneExists(zoneId)) {
        lastError = "Invalid animal fields or unknown zone";
        return false;
    }
    animal->setName(name);
    animal->setAge(age);
    animal->setGender(gender);
    animal->setZoneId(zoneId);
    return true;
}
