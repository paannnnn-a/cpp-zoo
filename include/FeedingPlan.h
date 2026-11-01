/**
 * @file FeedingPlan.h
 * @brief 喂养计划，描述一只动物在指定时间对一种食物的需求。
 */

#ifndef FEEDINGPLAN_H
#define FEEDINGPLAN_H

#include <string>

/**
 * @brief 喂养计划，描述一只动物在指定时间对一种食物的需求。
 *
 * @details 数量单位与对应库存食物一致，新建计划默认为未完成。
 */
class FeedingPlan {
private:
    std::string planId; ///< 计划编号。
    std::string animalId; ///< 动物编号。
    std::string foodId; ///< 库存食物编号。
    std::string date; ///< 计划日期，格式为 YYYY-MM-DD。
    std::string time; ///< 计划时间，格式为 HH:MM。
    double amount; ///< 计划喂养数量，单位与对应食物一致。
    bool completed; ///< 计划是否已完成。

public:

    /**
     * @brief 创建一条未完成的喂养计划。
     *
     * @param[in] planId 计划编号。
     * @param[in] animalId 动物编号。
     * @param[in] foodId 对应的库存食物编号。
     * @param[in] date 计划日期，格式为 YYYY-MM-DD。
     * @param[in] time 计划时间，格式为 HH:MM。
     * @param[in] amount 大于 0 的有限喂养数量，单位与对应食物一致。
     * @throws std::invalid_argument 数量不是有限数值，或小于或等于 0。
     * @note 本构造函数仅校验数量；其他字段由菜单和管理器的相关操作校验。
     */
    FeedingPlan(const std::string& planId,
                const std::string& animalId,
                const std::string& foodId,
                const std::string& date,
                const std::string& time,
                double amount);

    /**
     * @brief 获取计划编号。
     *
     * @return 计划编号。
     */
    std::string getPlanId() const;

    /**
     * @brief 获取动物编号。
     *
     * @return 计划对应的动物编号。
     */
    std::string getAnimalId() const;

    /**
     * @brief 获取食物编号。
     *
     * @return 计划对应的库存食物编号。
     */
    std::string getFoodId() const;

    /**
     * @brief 获取计划日期。
     *
     * @return 计划日期字符串。
     */
    std::string getDate() const;

    /**
     * @brief 获取计划时间。
     *
     * @return 计划时间字符串。
     */
    std::string getTime() const;

    /**
     * @brief 获取计划喂养数量。
     *
     * @return 以对应食物单位计量的喂养数量。
     */
    double getAmount() const;

    /**
     * @brief 查询计划是否已经完成。
     *
     * @retval true 计划已完成。
     * @retval false 计划未完成。
     */
    bool isCompleted() const;

    /**
     * @brief 将未完成的计划标记为已完成。
     *
     * @retval true 本次成功标记完成。
     * @retval false 计划此前已经完成。
     * @note 仅修改状态，不扣库存；实际执行喂养应使用 FeedingManager::completePlan()。
     */
    bool markCompleted();

    /**
     * @brief 输出计划字段和完成状态。
     */
    void display() const;
};

#endif
