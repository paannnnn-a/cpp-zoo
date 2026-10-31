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
     * @brief 通过初始化列表初始化成员或绑定协作对象；实体构造复用字段校验，派生动物委托 Animal 构造。
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
    // 基类指针销毁派生对象时，必须有虚析构函数。
    /**
     * @brief 使用默认析构释放成员资源；虚析构同时执行派生类析构。
     * @note 参数：无。
         */
    virtual ~Animal() = default;

    // 禁止整体赋值绕过“不能修改编号”的约定；用 Setter 修改业务字段。
    /**
     * @brief 通过初始化列表初始化成员或绑定协作对象；实体构造复用字段校验，派生动物委托 Animal 构造。
         */
    Animal(const Animal&) = delete;
    Animal& operator=(const Animal&) = delete;
    /**
     * @brief 通过初始化列表初始化成员或绑定协作对象；实体构造复用字段校验，派生动物委托 Animal 构造。
         */
    Animal(Animal&&) = delete;
    Animal& operator=(Animal&&) = delete;

    const std::string& getId() const;
    const std::string& getName() const;
    const std::string& getSpecies() const;
    /**
     * @brief 直接返回对应成员的值或只读引用；引用的有效期受当前对象生命周期限制。
     * @note 参数：无。
         */
    int getAge() const;
    const std::string& getGender() const;
    const std::string& getZoneId() const;
    /**
     * @brief 直接返回对应成员的值或只读引用；引用的有效期受当前对象生命周期限制。
     * @note 参数：无。
         */
    AnimalStatus getStatus() const;

    // 构造与 Setter 校验失败抛出 invalid_argument；不会留下非法数据。
    /**
     * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
     * @param name 名称。
         */
    void setName(const std::string& name);
    /**
     * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
     * @param species 物种。
         */
    void setSpecies(const std::string& species);
    /**
     * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
     * @param age 非负年龄。
         */
    void setAge(int age);
    /**
     * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
     * @param gender 性别文本。
         */
    void setGender(const std::string& gender);
    // 此处只校验编号格式。园区是否存在由阶段4的 AnimalManager 校验。
    /**
     * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
     * @param zoneId 所属园区编号。
         */
    void setZoneId(const std::string& zoneId);
    /**
     * @brief 先校验传入字段，校验通过后赋值；非法字段抛出 std::invalid_argument。
     * @param status 动物状态。
         */
    void setStatus(AnimalStatus status);

    /**
     * @brief 返回当前派生类对应的固定类别名称，供多态查询及持久化使用。
     * @note 参数：无。
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
