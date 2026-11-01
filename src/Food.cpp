/**
 * @file Food.cpp
 * @brief 实现食物基本信息访问、库存操作和预警判断。
 */

#include "../include/Food.h"
#include <iostream>

/**
 * @brief 初始化食物的基本信息和库存。
 */
Food::Food(const std::string& id,
           const std::string& name,
           double stock,
           const std::string& unit,
           double warningStock)
    : id(id),
      name(name),
      stock(stock),
      unit(unit),
      warningStock(warningStock) {
}

/**
 * @brief 增加食物库存，并输出操作结果。
 */
void Food::addStock(double amount) {
    if (amount <= 0) {
        std::cout << "入库数量必须大于0！" << std::endl;
        return;
    }

    stock += amount;
    std::cout << "入库成功！" << std::endl;
}

/**
 * @brief 在数量有效且库存足够时扣减库存。
 */
bool Food::reduceStock(double amount) {
    if (amount <= 0) {
        std::cout << "出库数量必须大于0！" << std::endl;
        return false;
    }

    if (amount > stock) {
        std::cout << "库存不足，无法出库！" << std::endl;
        return false;
    }

    stock -= amount;
    std::cout << "出库成功！" << std::endl;
    return true;
}

/**
 * @brief 判断库存是否已达到预警范围。
 */
bool Food::isLowStock() const {
    return stock <= warningStock;
}

/**
 * @brief 输出食物基本信息、保存建议和库存预警状态。
 */
void Food::displayInfo() const {
    std::cout << "食物编号：" << id << std::endl;
    std::cout << "食物名称：" << name << std::endl;
    std::cout << "食物类型：" << getType() << std::endl;
    std::cout << "当前库存：" << stock << unit << std::endl;
    std::cout << "预警值：" << warningStock << unit << std::endl;
    std::cout << "保存要求：" << getStorageAdvice() << std::endl;

    if (isLowStock()) {
        std::cout << "库存状态：库存不足，请及时补充！" << std::endl;
    } else {
        std::cout << "库存状态：正常" << std::endl;
    }
}

/**
 * @brief 获取食物编号。
 */
std::string Food::getId() const {
    return id;
}

/**
 * @brief 获取食物名称。
 */
std::string Food::getName() const {
    return name;
}

/**
 * @brief 获取当前库存数量。
 */
double Food::getStock() const {
    return stock;
}

/**
 * @brief 获取计量单位。
 */
std::string Food::getUnit() const {
    return unit;
}

/**
 * @brief 获取库存预警阈值。
 */
double Food::getWarningStock() const {
    return warningStock;
}

/**
 * @brief 更新食物名称。
 */
void Food::setName(const std::string& newName) {
    if (!newName.empty()) {
        name = newName;
    }
}

/**
 * @brief 更新库存预警阈值。
 */
void Food::setWarningStock(double newWarningStock) {
    if (newWarningStock >= 0) {
        warningStock = newWarningStock;
    }
}
