/**
 * @file Food.h
 * @brief 食物抽象基类，封装库存数量、计量单位和预警值。
 */

#ifndef FOOD_H
#define FOOD_H

#include <string>

/**
 * @brief 食物抽象基类，封装库存数量、计量单位和预警值。
 *
 * @details 通过虚函数提供食物类型和保存建议，供库存管理器统一管理。
 */
class Food {
private:
    std::string id; ///< 食物编号。
    std::string name; ///< 食物名称。
    double stock; ///< 当前库存数量。
    std::string unit; ///< 计量单位。
    double warningStock; ///< 库存预警阈值，单位与库存一致。

public:

    /**
     * @brief 初始化食物的基本信息和库存。
     *
     * @param[in] id 食物编号。
     * @param[in] name 食物名称。
     * @param[in] stock 初始库存数量。
     * @param[in] unit 计量单位。
     * @param[in] warningStock 库存预警阈值。
     * @note 构造函数直接保存参数，调用方负责校验编号和库存等字段。
     */
    Food(const std::string& id,
         const std::string& name,
         double stock,
         const std::string& unit,
         double warningStock);

    /**
     * @brief 通过基类指针销毁派生食物对象。
     */
    virtual ~Food() = default;

    /**
     * @brief 增加食物库存，并输出操作结果。
     *
     * @param[in] amount 入库数量，单位与库存一致。
     * @note 数量小于或等于 0 时不修改库存；有限数值和溢出检查由调用方负责。
     */
    void addStock(double amount);

    /**
     * @brief 在数量有效且库存足够时扣减库存。
     *
     * @param[in] amount 出库数量，单位与库存一致。
     * @retval true 成功扣减库存。
     * @retval false 数量小于或等于 0，或库存不足。
     * @note 调用方应保证数量为有限数值。
     */
    bool reduceStock(double amount);

    /**
     * @brief 判断库存是否已达到预警范围。
     *
     * @retval true 库存小于或等于预警阈值。
     * @retval false 库存高于预警阈值。
     */
    bool isLowStock() const;

    /**
     * @brief 输出食物基本信息、保存建议和库存预警状态。
     */
    virtual void displayInfo() const;

    /**
     * @brief 获取食物类型名称，由派生类实现。
     *
     * @return 食物类型名称。
     */
    virtual std::string getType() const = 0;

    /**
     * @brief 获取食物保存建议，由派生类实现。
     *
     * @return 保存条件的文字说明。
     */
    virtual std::string getStorageAdvice() const = 0;

    /**
     * @brief 获取食物编号。
     *
     * @return 食物编号。
     */
    std::string getId() const;

    /**
     * @brief 获取食物名称。
     *
     * @return 食物名称。
     */
    std::string getName() const;

    /**
     * @brief 获取当前库存数量。
     *
     * @return 以 getUnit() 为单位的库存数量。
     */
    double getStock() const;

    /**
     * @brief 获取计量单位。
     *
     * @return 库存及出入库操作使用的单位。
     */
    std::string getUnit() const;

    /**
     * @brief 获取库存预警阈值。
     *
     * @return 以 getUnit() 为单位的预警阈值。
     */
    double getWarningStock() const;

    /**
     * @brief 更新食物名称。
     *
     * @param[in] newName 新的食物名称；空字符串将被忽略。
     */
    void setName(const std::string& newName);

    /**
     * @brief 更新库存预警阈值。
     *
     * @param[in] newWarningStock 新的非负预警阈值；负值将被忽略。
     */
    void setWarningStock(double newWarningStock);
};

#endif
