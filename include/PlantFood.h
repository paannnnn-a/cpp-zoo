/**
 * @file PlantFood.h
 * @brief 植物类食物，提供植物饲料的类型名称和保存建议。
 */

#ifndef PLANTFOOD_H
#define PLANTFOOD_H

#include "Food.h"

/**
 * @brief 植物类食物，提供植物饲料的类型名称和保存建议。
 */
class PlantFood : public Food {
public:
    /**
     * @copydoc Food::Food
     */
    using Food::Food;

    /**
     * @brief 获取植物类食物的类型名称。
     *
     * @return 类型名称“植物类”。
     */
    std::string getType() const override;

    /**
     * @brief 获取植物类食物的保存建议。
     *
     * @return 阴凉、通风、防潮保存。
     */
    std::string getStorageAdvice() const override;
};

#endif
