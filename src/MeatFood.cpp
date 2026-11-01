/**
 * @file MeatFood.cpp
 * @brief 实现肉类食物的类型名称和保存建议。
 */

#include "../include/MeatFood.h"

/**
 * @brief 获取肉类食物的类型名称。
 */
std::string MeatFood::getType() const {
    return "肉类";
}

/**
 * @brief 获取肉类食物的保存建议。
 */
std::string MeatFood::getStorageAdvice() const {
    return "需要冷藏或冷冻保存";
}
