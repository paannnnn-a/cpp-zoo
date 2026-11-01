/**
 * @file InventoryManager.h
 * @brief 库存管理器，管理食物对象、出入库和文件持久化。
 */

#ifndef INVENTORYMANAGER_H
#define INVENTORYMANAGER_H

#include <memory>
#include <string>
#include <vector>

#include "Food.h"

/**
 * @brief 库存管理器，管理食物对象、出入库和文件持久化。
 *
 * @details 通过 std::unique_ptr 独占食物对象的所有权。
 */
class InventoryManager {
private:
    std::vector<std::unique_ptr<Food>> foods; ///< 独占管理的食物对象集合。

public:

    /**
     * @brief 接管一个食物对象并加入库存。
     *
     * @param[in] food 待添加的食物对象，调用时转移所有权。
     * @retval true 食物已加入库存。
     * @retval false 指针为空、编号为空或编号重复。
     * @note 参数按值接收，即使添加失败，传入对象的所有权也不会返还调用方。
     */
    bool addFood(std::unique_ptr<Food> food);

    /**
     * @brief 按食物编号查找库存对象。
     *
     * @param[in] foodId 待查询的食物编号。
     * @return 匹配的非拥有指针；未找到时返回 nullptr。
     * @note 指针由本管理器持有的对象提供，在管理器销毁或成功重新加载库存后失效。
     */
    Food* findFood(const std::string& foodId);

    /**
     * @brief 校验入库数量并增加指定食物的库存。
     *
     * @param[in] foodId 食物编号。
     * @param[in] amount 大于 0 的有限入库数量。
     * @retval true 入库成功。
     * @retval false 食物不存在、数量非法或库存相加后溢出。
     */
    bool stockIn(const std::string& foodId, double amount);

    /**
     * @brief 校验出库数量并扣减指定食物的库存。
     *
     * @param[in] foodId 食物编号。
     * @param[in] amount 大于 0 的有限出库数量。
     * @retval true 出库成功。
     * @retval false 食物不存在、数量非法或库存不足。
     */
    bool stockOut(const std::string& foodId, double amount);

    /**
     * @brief 输出全部食物及其库存信息。
     */
    void showAllFoods() const;

    /**
     * @brief 输出库存小于或等于预警阈值的食物。
     */
    void showLowStockFoods() const;

    /**
     * @brief 将全部库存以竖线分隔的文本格式覆盖写入文件。
     *
     * @param[in] filename 目标库存文件路径。
     * @retval true 文件写入并关闭成功。
     * @retval false 文本字段含分隔符或换行符，或文件操作失败。
     * @note 每行字段依次为编号、名称、类型、库存、单位和预警阈值。
     */
    bool saveToFile(const std::string& filename) const;

    /**
     * @brief 校验并加载库存文件，全部成功后替换当前库存。
     *
     * @param[in] filename 待读取的库存文件路径。
     * @retval true 全部库存数据加载成功。
     * @retval false 文件操作失败，或字段、编号、类型、数量校验失败。
     * @note 读取失败时保留原库存；成功加载会使先前返回的食物指针失效。
     */
    bool loadFromFile(const std::string& filename);
};

#endif
