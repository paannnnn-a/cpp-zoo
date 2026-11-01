/**
 * @file FeedingRecord.cpp
 * @brief 实现历史喂养记录的数据访问和信息输出。
 */

#include "../include/FeedingRecord.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

/**
 * @brief 创建一条实际喂养记录。
 */
FeedingRecord::FeedingRecord(
    const std::string& recordId,
    const std::string& planId,
    const std::string& animalId,
    const std::string& foodId,
    double amount,
    const std::string& unit,
    const std::string& keeperName,
    const std::string& completedAt)
    : recordId(recordId),
      planId(planId),
      animalId(animalId),
      foodId(foodId),
      amount(amount),
      unit(unit),
      keeperName(keeperName),
      completedAt(completedAt) {

    if (!std::isfinite(amount) || amount <= 0) {
        throw std::invalid_argument(
            "实际喂养数量必须是大于0的有限数值"
        );
    }
}

/**
 * @brief 获取记录编号。
 */
std::string FeedingRecord::getRecordId() const {
    return recordId;
}

/**
 * @brief 获取对应的计划编号。
 */
std::string FeedingRecord::getPlanId() const {
    return planId;
}

/**
 * @brief 获取动物编号。
 */
std::string FeedingRecord::getAnimalId() const {
    return animalId;
}

/**
 * @brief 获取食物编号。
 */
std::string FeedingRecord::getFoodId() const {
    return foodId;
}

/**
 * @brief 获取实际喂养数量。
 */
double FeedingRecord::getAmount() const {
    return amount;
}

/**
 * @brief 获取执行时的计量单位。
 */
std::string FeedingRecord::getUnit() const {
    return unit;
}

/**
 * @brief 获取操作人员姓名。
 */
std::string FeedingRecord::getKeeperName() const {
    return keeperName;
}

/**
 * @brief 获取实际完成时间。
 */
std::string FeedingRecord::getCompletedAt() const {
    return completedAt;
}

/**
 * @brief 输出完整的实际喂养记录。
 */
void FeedingRecord::display() const {
    std::cout << "记录编号：" << recordId << "\n";
    std::cout << "计划编号：" << planId << "\n";
    std::cout << "动物编号：" << animalId << "\n";
    std::cout << "食物编号：" << foodId << "\n";
    std::cout << "实际喂养量：" << amount << unit << "\n";
    std::cout << "操作人员：" << keeperName << "\n";
    std::cout << "完成时间：" << completedAt << "\n";
}
