/**
 * @file FeedingMenu.h
 * @brief 喂养交互菜单，处理计划录入、执行、查询和统计。
 */

#ifndef FEEDINGMENU_H
#define FEEDINGMENU_H

#include "FeedingManager.h"

/**
 * @brief 喂养交互菜单，处理计划录入、执行、查询和统计。
 *
 * @details 引用已有饲养管理器，由主程序负责统一保存计划和记录。
 */
class FeedingMenu {
private:
    FeedingManager& feeding; ///< 共享的饲养管理器引用。

    /**
     * @brief 输出喂养管理菜单选项。
     */
    void showMenu() const;

    /**
     * @brief 交互录入并校验喂养计划，确认后添加。
     *
     * @note 录入过程中输入 /cancel 可取消。
     */
    void addPlan();

    /**
     * @brief 读取日期并显示该日期的计划。
     *
     * @note 输入 0 或输入结束时返回。
     */
    void queryByDate();

    /**
     * @brief 读取动物编号并显示该动物的计划。
     *
     * @note 输入 0 或输入结束时返回。
     */
    void queryByAnimal();

    /**
     * @brief 读取计划编号和人员姓名，确认后请求执行喂养。
     *
     * @note 完成前可取消；库存、计划和记录由 FeedingManager::completePlan() 更新。
     */
    void completePlan();

public:

    /**
     * @brief 初始化喂养菜单并绑定饲养管理器。
     *
     * @param[in] manager 共享饲养管理器，其生命周期必须覆盖本菜单。
     */
    explicit FeedingMenu(FeedingManager& manager);

    /**
     * @brief 运行喂养菜单循环，直到请求返回或退出。
     *
     * @return '0' 表示返回上一页，'M' 表示返回主菜单，'Q' 表示退出请求或输入结束。
     */
    char run();
};

#endif
