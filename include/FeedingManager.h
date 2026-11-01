/**
 * @file FeedingManager.h
 * @brief 饲养管理器，管理喂养计划、历史记录和消耗统计。
 */

#ifndef FEEDINGMANAGER_H
#define FEEDINGMANAGER_H

#include <string>
#include <vector>

#include "FeedingPlan.h"
#include "InventoryManager.h"
#include "FeedingRecord.h"

/**
 * @brief 饲养管理器，管理喂养计划、历史记录和消耗统计。
 *
 * @details 与库存管理器共享库存，成功执行计划时扣减库存并生成记录。
 */
class FeedingManager {
private:
    std::vector<FeedingPlan> plans; ///< 已加载或新建的喂养计划。
    std::vector<FeedingRecord> records; ///< 已完成喂养的历史记录。

    InventoryManager& inventory; ///< 与库存模块共享的库存管理器引用。

    /**
     * @brief 按编号查找可修改的喂养计划。
     *
     * @param[in] planId 计划编号。
     * @return 匹配的非拥有指针；未找到时返回 nullptr。
     * @note 指针可能在后续添加计划、成功加载计划或管理器销毁后失效。
     */
    FeedingPlan* findPlan(const std::string& planId);

public:

    /**
     * @brief 绑定已有库存管理器，初始化饲养管理器。
     *
     * @param[in] manager 共享库存管理器，其生命周期必须覆盖本管理器。
     */
    explicit FeedingManager(InventoryManager& manager);

    /**
     * @brief 校验并添加一条未完成的喂养计划。
     *
     * @param[in] plan 待添加的计划，成功时复制到计划集合。
     * @retval true 计划添加成功。
     * @retval false 编号非法或重复、计划已完成、数量非法或食物不存在。
     * @note 添加计划不扣减库存，当前库存不足时仅输出提醒。
     */
    bool addPlan(const FeedingPlan& plan);

    /**
     * @brief 输出全部喂养计划。
     */
    void showAllPlans() const;

    /**
     * @brief 输出指定计划日期的喂养计划。
     *
     * @param[in] date 待匹配的计划日期，格式为 YYYY-MM-DD。
     */
    void showPlansByDate(const std::string& date) const;

    /**
     * @brief 输出指定动物的喂养计划。
     *
     * @param[in] animalId 待查询的动物编号。
     */
    void showPlansByAnimal(const std::string& animalId) const;

    /**
     * @brief 执行喂养计划，成功扣减库存后标记完成并保留记录。
     *
     * @param[in] planId 待执行的计划编号。
     * @param[in] keeperName 操作人员姓名，不能为空白或包含竖线、换行符。
     * @retval true 计划完成，库存已扣减且记录已生成。
     * @retval false 人员、计划、食物、库存、记录或系统时间检查失败，或出库失败。
     * @note 记录编号为 FR- 加计划编号；每条计划只能成功执行一次。
     * @note 出库失败时移除本次预先创建的记录。此操作只更新内存，不自动保存文件。
     */
    bool completePlan(const std::string& planId,
                      const std::string& keeperName);

    /**
     * @brief 将全部喂养计划及完成状态覆盖写入文件。
     *
     * @param[in] filename 目标计划文件路径。
     * @retval true 文件写入并关闭成功。
     * @retval false 文本字段含分隔符或换行符，或文件操作失败。
     * @note 每行字段依次为计划编号、动物编号、食物编号、日期、时间、数量和完成状态。
     */
    bool saveToFile(const std::string& filename) const;

    /**
     * @brief 校验并加载全部计划，全部成功后替换当前计划。
     *
     * @param[in] filename 待读取的计划文件路径。
     * @retval true 全部计划加载成功。
     * @retval false 文件操作失败，或计划字段、编号、日期、时间、数量、状态、食物引用校验失败。
     * @note 应先加载库存；读取失败保留原计划，恢复完成状态时不扣减库存。
     */
    bool loadFromFile(const std::string& filename);

    /**
     * @brief 输出全部历史喂养记录。
     */
    void showAllRecords() const;

    /**
     * @brief 输出指定动物的历史喂养记录。
     *
     * @param[in] animalId 待查询的动物编号。
     */
    void showRecordsByAnimal(
        const std::string& animalId) const;

    /**
     * @brief 将全部历史喂养记录覆盖写入文件。
     *
     * @param[in] filename 目标记录文件路径。
     * @retval true 文件写入并关闭成功。
     * @retval false 文本字段或数量非法，或文件操作失败。
     * @note 每行字段依次为记录编号、计划编号、动物编号、食物编号、数量、单位、人员和完成时间。
     */
    bool saveRecordsToFile(
        const std::string& filename) const;

    /**
     * @brief 校验并加载历史记录，全部成功后替换当前记录。
     *
     * @param[in] filename 待读取的记录文件路径。
     * @retval true 全部历史记录加载成功。
     * @retval false 文件操作失败，或字段、编号、数量、时间、计划一致性校验失败。
     * @note 应先加载库存和计划；只恢复记录，不执行喂养，也不扣减库存。
     * @note 读取失败时保留原记录。
     */
    bool loadRecordsFromFile(
        const std::string& filename);

    /**
     * @brief 按系统本地日期输出今日喂养任务及完成数量。
     */
    void showTodayPlans() const;

    /**
     * @brief 按食物编号和单位汇总全部历史喂养消耗。
     *
     * @note 同一食物的不同计量单位分别统计，累计数量溢出时停止输出统计结果。
     */
    void showFoodConsumption() const;

    /**
     * @brief 按实际完成日期汇总食物消耗。
     *
     * @param[in] date 实际完成日期，格式为 YYYY-MM-DD。
     * @note 校验日期有效性，并按食物编号和计量单位分别累计。
     */
    void showFoodConsumptionByDate(
        const std::string& date) const;
};

#endif
