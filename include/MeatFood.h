/**
 * @file MeatFood.h
 * @brief 肉类食物，提供肉类饲料的类型名称和保存建议。
 */

#ifndef MEATFOOD_H
#define MEATFOOD_H

#include "Food.h"

/**
 * @brief 肉类食物，提供肉类饲料的类型名称和保存建议。
 */
class MeatFood : public Food {
public:
    /**
     * @copydoc Food::Food
     */
    using Food::Food;

    /**
     * @brief 获取肉类食物的类型名称。
     *
     * @return 类型名称“肉类”。
     */
    std::string getType() const override;

    /**
     * @brief 获取肉类食物的保存建议。
     *
     * @return 冷藏或冷冻保存。
     */
    std::string getStorageAdvice() const override;
};

#endif
