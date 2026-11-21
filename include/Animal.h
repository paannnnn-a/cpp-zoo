#ifndef ANIMAL_H
#define ANIMAL_H

#include <string>
#include "AnimalCommon.h"

// 抽象基类不能直接创建对象，实际对象应为 Mammal、Bird 或 Reptile。
/**
 * @brief 抽象动物基类；保存编号、名称、物种、年龄、性别、园区和状态。Getter 提供只读访问，Setter 校验后修改；Mammal、Bird、Reptile 公有派生并实现 getCategory，虚析构支持多态销毁。
 */
class Animal {
public:
    /**
     * @brief 初始化动物编号，并按各字段的修改规则校验其余资料。
     * @param animalId 动物唯一编号。
     * @param name 名称。
     * @param species 物种。
     * @param age 非负年龄。
     * @param gender 性别文本。
     * @param zoneId 所属园区编号。
     * @param status 动物状态。
     */
    Animal(const std::string& animalId, const std::string& name,
           const std::string& species, int age, const std::string& gender,
           const std::string& zoneId, AnimalStatus status = AnimalStatus::NORMAL);
    /// @brief 通过 Animal 指针删除对象时，虚析构函数会调用实际派生类的析构函数。
    virtual ~Animal() = default;

    // 禁止整体赋值绕过“不能修改编号”的约定；用 Setter 修改业务字段。
    // 禁止拷贝构造和拷贝赋值。
    Animal(const Animal&) = delete;
    Animal& operator=(const Animal&) = delete;
    // 禁止移动构造和移动赋值。
    Animal(Animal&&) = delete;
    Animal& operator=(Animal&&) = delete;

    const std::string& getId() const;
    const std::string& getName() const;
    const std::string& getSpecies() const;
    int getAge() const;
    const std::string& getGender() const;
    const std::string& getZoneId() const;
    AnimalStatus getStatus() const;

    // 构造与 Setter 校验失败抛出 invalid_argument；不会留下非法数据。
    /**
     * @brief 名称须包含非空白字符，且不能包含竖线或换行。
     * @throws std::invalid_argument 字段校验失败。
     */
    void setName(const std::string& name);
    /**
     * @brief 物种须包含非空白字符，且不能包含竖线或换行。
     * @throws std::invalid_argument 字段校验失败。
     */
    void setSpecies(const std::string& species);
    /**
     * @brief 年龄必须是非负整数。
     * @throws std::invalid_argument 字段校验失败。
     */
    void setAge(int age);
    /**
     * @brief 性别须为非空白文本，且不能包含竖线或换行。
     * @throws std::invalid_argument 字段校验失败。
     */
    void setGender(const std::string& gender);
    /**
     * @brief 校验园区编号格式；园区是否存在由 AnimalManager 检查。
     * @throws std::invalid_argument 字段校验失败。
     */
    void setZoneId(const std::string& zoneId);
    /**
     * @brief 状态须为 AnimalStatus 定义的枚举值。
     * @throws std::invalid_argument 字段校验失败。
     */
    void setStatus(AnimalStatus status);

    /**
     * @brief 返回当前派生类对应的固定类别名称，供多态查询及持久化使用。
     */
    virtual std::string getCategory() const = 0;

private:
    // 不提供 setAnimalId，避免喂养/医疗历史失去对应关系。
    const std::string animalId;
    std::string name;
    std::string species;
    int age;
    std::string gender;
    std::string zoneId;
    AnimalStatus status;
};
#endif
