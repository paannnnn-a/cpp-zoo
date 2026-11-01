/**
 * @file InventoryMenu.h
 * @brief 库存交互菜单，处理食物查询、编辑和出入库操作。
 */

#ifndef INVENTORYMENU_H
#define INVENTORYMENU_H

#include <string>
#include "InventoryManager.h"

/**
 * @brief 库存交互菜单，处理食物查询、编辑和出入库操作。
 *
 * @details 引用已有库存管理器，数据变更后尝试保存到指定文件。
 */
class InventoryMenu {
private:
    InventoryManager& inventory; ///< 共享的库存管理器引用。

    std::string dataFile; ///< 库存数据文件路径。

    /**
     * @brief 输出库存管理菜单选项。
     */
    void showMenu() const;

    /**
     * @brief 读取食物编号并显示匹配的库存信息。
     *
     * @note 输入 0 或输入结束时返回。
     */
    void queryFood();

    /**
     * @brief 交互录入一种食物，确认后加入库存并尝试保存。
     *
     * @note 录入过程中输入 /cancel 可取消。
     */
    void addFood();

    /**
     * @brief 交互修改食物名称和预警阈值，确认后尝试保存。
     *
     * @note 输入 /cancel 可取消，空输入保留原字段。
     */
    void editFood();

    /**
     * @brief 交互处理出入库，确认后修改库存并尝试保存。
     *
     * @param[in] isStockIn 为 true 时入库，为 false 时出库。
     * @note 输入 0 或输入结束时取消当前操作。
     */
    void changeStock(bool isStockIn);

public:

    /**
     * @brief 初始化库存菜单并绑定库存管理器。
     *
     * @param[in] manager 共享库存管理器，其生命周期必须覆盖本菜单。
     * @param[in] filename 库存数据文件路径。
     */
    InventoryMenu(InventoryManager& manager,
                  const std::string& filename);

    /**
     * @brief 运行库存菜单循环，直到请求返回或退出。
     *
     * @return '0' 表示返回上一页，'M' 表示返回主菜单，'Q' 表示退出请求或输入结束。
     */
    char run();
};

#endif
