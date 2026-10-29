#include <iostream>
#include <string>

#include "include/ZoneManager.h"
#include "include/AnimalManager.h"
#include "include/HealthManager.h"
#include "include/AnimalMenu.h"
#include "include/HealthMenu.h"
#include "include/AStorage.h"

#include "include/InventoryManager.h"
#include "include/InventoryMenu.h"
#include "include/FeedingManager.h"
#include "include/FeedingMenu.h"

/**
 * 功能：创建并加载A/B子系统，调度菜单，统一保存和退出。
 *
 * 参数：无。
 *
 * 返回：
 * 0：数据保存成功，正常退出。
 * 1：启动加载失败，或输入结束后仍无法完成保存。
 */
int main() {
    const std::string dataDirectory = "data";
    const std::string foodFilename = "data/foods.txt";
    const std::string planFilename = "data/feeding_plans.txt";
    const std::string recordFilename = "data/feeding_records.txt";

    // =========================================================
    // 一、创建并加载A：园区、动物、医疗
    // =========================================================

    // 对象创建顺序必须保证被引用的对象先创建、后销毁。
    ZoneManager zones;
    AnimalManager animals(zones);
    HealthManager health(animals);

    // 负责A的园区、动物、医疗三个文件的加载和保存。
    AStorage aStorage(zones, animals, health, dataDirectory);

    // 内部按 zones → animals → medical_records 顺序加载。
    // 动物必须先于B的喂养计划加载。
    if (!aStorage.load()) {
        std::cout
            << "动物与健康数据加载失败："
            << aStorage.getLastError()
            << '\n';

        return 1;
    }

    // =========================================================
    // 二、创建并加载B：库存、喂养计划、喂养记录
    // =========================================================

    InventoryManager inventory;

    if (!inventory.loadFromFile(foodFilename)) {
        std::cout << "库存加载失败，请检查数据文件。\n";
        return 1;
    }

    // 保留你当前B模块的单参数构造函数。
    // 如果B以后完成动物接口接入，并新增双参数构造函数，
    // 再将下一行改为：
    // FeedingManager feeding(inventory, animals);
    FeedingManager feeding(inventory);

    // 必须先加载库存，再加载引用食物的喂养计划。
    if (!feeding.loadFromFile(planFilename)) {
        std::cout << "计划加载失败，请检查计划文件。\n";
        return 1;
    }

    if (!feeding.loadRecordsFromFile(recordFilename)) {
        std::cout << "喂养记录加载失败，请检查记录文件。\n";
        return 1;
    }

    // =========================================================
    // 三、创建菜单，共享前面已经加载的Manager
    // =========================================================

    InventoryMenu inventoryMenu(inventory, foodFilename);
    FeedingMenu feedingMenu(feeding);

    AnimalMenu animalMenu(animals, zones, aStorage);
    HealthMenu healthMenu(animals, health, aStorage);

    /**
     * 功能：统一保存A/B数据。
     *
     * 参数：无，通过引用捕获使用前面创建的Manager和文件路径。
     *
     * 返回：
     * true：所有保存操作均成功。
     * false：至少一个保存操作失败。
     *
     * 说明：某项失败后仍尝试保存其他项，不提前返回。
     */
    const auto saveAll = [&]() -> bool {
        bool success = true;

        if (!aStorage.save()) {
            std::cout
                << "动物与健康数据保存失败："
                << aStorage.getLastError()
                << '\n';

            success = false;
        }

        if (!inventory.saveToFile(foodFilename)) {
            std::cout
                << "库存保存失败，请检查文件路径和写入权限。\n";

            success = false;
        }

        if (!feeding.saveToFile(planFilename)) {
            std::cout
                << "计划保存失败，请检查文件路径和写入权限。\n";

            success = false;
        }

        if (!feeding.saveRecordsToFile(recordFilename)) {
            std::cout
                << "喂养记录保存失败，请检查文件路径和写入权限。\n";

            success = false;
        }

        return success;
        };

    /**
     * 功能：确认用户是否请求退出系统。
     *
     * 参数：无。
     *
     * 返回：
     * true：用户输入Y/y，或标准输入已经结束。
     * false：用户输入其他内容，取消退出。
     *
     * 说明：输入结束时不反复询问，但仍需尝试保存数据。
     */
    const auto confirmExit = []() -> bool {
        if (!std::cin.good()) {
            return true;
        }

        std::cout
            << "确定保存并退出吗？"
            << "输入Y确认，其他输入取消：";

        std::string answer;

        if (!std::getline(std::cin, answer)) {
            return true;
        }

        return answer == "Y" || answer == "y";
        };

    // =========================================================
    // 四、系统主菜单
    // =========================================================

    bool shouldExit = false;

    while (!shouldExit) {
        std::cout << "\n========== 动物园综合管理系统 ==========\n";
        std::cout << "1. 食物库存管理\n";
        std::cout << "2. 喂养计划管理\n";
        std::cout << "3. 动物管理\n";
        std::cout << "4. 动物健康管理\n";
        std::cout << "S. 保存全部数据\n";
        std::cout << "Q. 退出系统\n";
        std::cout << "请选择：";

        std::string choice;
        bool exitRequested = false;

        if (!std::getline(std::cin, choice)) {
            // 输入结束也要进入统一保存流程，不能直接跳过保存。
            exitRequested = true;
        }
        else if (
            choice == "1" ||
            choice == "2" ||
            choice == "3" ||
            choice == "4"
            ) {
            char result = '0';

            if (choice == "1") {
                result = inventoryMenu.run();
            }
            else if (choice == "2") {
                result = feedingMenu.run();
            }
            else if (choice == "3") {
                result = animalMenu.run();
            }
            else {
                result = healthMenu.run();
            }

            // A菜单已经有自动保存；这里再做一次统一保存，
            // 同时保留原程序离开B菜单后保存数据的行为。
            if (!saveAll()) {
                std::cout
                    << "部分数据尚未保存成功，"
                    << "请处理问题后选择S重试。\n";
            }

            if (result == 'Q') {
                exitRequested = confirmExit();
            }

            // 返回'0'或'M'时，重新显示系统主菜单。
        }
        else if (choice == "S" || choice == "s") {
            if (saveAll()) {
                std::cout << "全部数据已保存。\n";
            }
            else {
                std::cout << "部分数据保存失败，请处理后重试。\n";
            }
        }
        else if (choice == "Q" || choice == "q") {
            exitRequested = confirmExit();
        }
        else if (
            choice == "0" ||
            choice == "M" ||
            choice == "m"
            ) {
            // 当前已经是系统主菜单，继续显示即可。
            continue;
        }
        else {
            std::cout << "无效选项，请重新输入。\n";
        }

        // 所有退出请求都从这里统一处理。
        if (exitRequested) {
            if (saveAll()) {
                shouldExit = true;
            }
            else {
                std::cout << "退出前保存失败。\n";

                if (!std::cin.good()) {
                    // 已无法继续交互，通过非零返回值报告失败。
                    return 1;
                }

                // 保留当前内存数据，允许修复路径/权限后重试。
                std::cout
                    << "暂不退出。请处理问题后选择S保存，"
                    << "或再次选择Q退出。\n";
            }
        }
    }

    std::cout << "全部数据已保存，程序已结束。\n";
    return 0;
}