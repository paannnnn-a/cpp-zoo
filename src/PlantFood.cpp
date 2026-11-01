/**
 * @file PlantFood.cpp
 * @brief 实现植物类食物的类型名称和保存建议。
 */

#include "../include/PlantFood.h"

/**
 * @brief 获取植物类食物的类型名称。
 */
std::string PlantFood::getType() const {
    return "植物类";
}

/**
 * @brief 获取植物类食物的保存建议。
 */
std::string PlantFood::getStorageAdvice() const {
    return "阴凉、通风、防潮保存";
}
