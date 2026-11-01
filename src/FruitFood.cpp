/**
 * @file FruitFood.cpp
 * @brief 实现水果类食物的类型名称和保存建议。
 */

#include "../include/FruitFood.h"

/**
 * @brief 获取水果类食物的类型名称。
 */
std::string FruitFood::getType() const {
    return "水果类";
}

/**
 * @brief 获取水果类食物的保存建议。
 */
std::string FruitFood::getStorageAdvice() const {
    return "冷藏保存，并注意保鲜";
}
