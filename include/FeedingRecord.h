/**
 * @file FeedingRecord.h
 * @brief 喂养历史记录，保存一次实际完成的喂养信息。
 */

#ifndef FEEDINGRECORD_H
#define FEEDINGRECORD_H

#include <string>

/**
 * @brief 喂养历史记录，保存一次实际完成的喂养信息。
 *
 * @details 保留执行时的数量、单位、操作人员和完成时间。
 */
class FeedingRecord {
private:
    std::string recordId; ///< 喂养记录编号。
    std::string planId; ///< 对应的计划编号。
    std::string animalId; ///< 动物编号。
    std::string foodId; ///< 食物编号。
    double amount; ///< 实际喂养数量。
    std::string unit; ///< 执行喂养时的计量单位。
    std::string keeperName; ///< 执行喂养的操作人员姓名。
    std::string completedAt; ///< 实际完成时间，格式为 YYYY-MM-DD HH:MM:SS。

public:

    /**
     * @brief 创建一条实际喂养记录。
     *
     * @param[in] recordId 记录编号。
     * @param[in] planId 对应的计划编号。
     * @param[in] animalId 动物编号。
     * @param[in] foodId 食物编号。
     * @param[in] amount 大于 0 的有限实际喂养数量。
     * @param[in] unit 执行时使用的计量单位。
     * @param[in] keeperName 执行喂养的操作人员姓名。
     * @param[in] completedAt 实际完成时间，格式为 YYYY-MM-DD HH:MM:SS。
     * @throws std::invalid_argument 数量不是有限数值，或小于或等于 0。
     * @note 本构造函数仅校验数量；文本字段由管理器的相关操作校验。
     */
    FeedingRecord(const std::string& recordId,
                  const std::string& planId,
                  const std::string& animalId,
                  const std::string& foodId,
                  double amount,
                  const std::string& unit,
                  const std::string& keeperName,
                  const std::string& completedAt);

    /**
     * @brief 获取记录编号。
     *
     * @return 记录编号。
     */
    std::string getRecordId() const;

    /**
     * @brief 获取对应的计划编号。
     *
     * @return 计划编号。
     */
    std::string getPlanId() const;

    /**
     * @brief 获取动物编号。
     *
     * @return 动物编号。
     */
    std::string getAnimalId() const;

    /**
     * @brief 获取食物编号。
     *
     * @return 食物编号。
     */
    std::string getFoodId() const;

    /**
     * @brief 获取实际喂养数量。
     *
     * @return 以 getUnit() 为单位的实际喂养数量。
     */
    double getAmount() const;

    /**
     * @brief 获取执行时的计量单位。
     *
     * @return 记录中保存的计量单位。
     */
    std::string getUnit() const;

    /**
     * @brief 获取操作人员姓名。
     *
     * @return 执行喂养的操作人员姓名。
     */
    std::string getKeeperName() const;

    /**
     * @brief 获取实际完成时间。
     *
     * @return 完成时间字符串。
     */
    std::string getCompletedAt() const;

    /**
     * @brief 输出完整的实际喂养记录。
     */
    void display() const;
};

#endif
