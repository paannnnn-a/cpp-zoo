/**
 * @file FruitFood.h
 * @brief 水果类食物，提供水果饲料的类型名称和保存建议。
 */

#ifndef FRUITFOOD_H
#define FRUITFOOD_H

#include "Food.h"

/**
 * @brief 水果类食物，提供水果饲料的类型名称和保存建议。
 */
class FruitFood : public Food {
public:
    /**
     * @copydoc Food::Food
     */
    using Food::Food;

    /**
     * @brief 获取水果类食物的类型名称。
     *
     * @return 类型名称“水果类”。
     */
    std::string getType() const override;

    /**
     * @brief 获取水果类食物的保存建议。
     *
     * @return 冷藏保存，并注意保鲜。
     */
    std::string getStorageAdvice() const override;
};

#endif
