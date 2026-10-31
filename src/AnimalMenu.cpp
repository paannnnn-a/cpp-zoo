#include "AnimalMenu.h"
#include "ConsoleInput.h"
using namespace zoo_ui;
/**
 * @brief 循环显示菜单、校验输入、执行确认后的操作并自动保存。
 * @note 参数：无。
 * @return '0'返回上级；'M'返回主菜单；'Q'请求统一退出。
 * @note 录入中的0仅取消当前操作；EOF按Q传播；保存失败保留内存并提示重试。
 * @details 循环显示选项并读取输入；查询直接显示，修改经确认后调用管理器并保存；捕获导航和字段异常。
 */
char AnimalMenu::run() {
    for (;;) {
        std::cout << "\n=== 动物管理 ===\n1添加 2修改资料 3移动园区 4修改状态\n5编号查询 6模糊查询 "
                     "7园区查询 8全部档案\n9园区列表 A添加园区\n";
        bool inForm = false;
        try {
            auto choice = ask("请选择");
            inForm = true;
            bool changed = false;
            if (choice == "1") {
                auto category = ask("类别 Mammal/Bird/Reptile");
                auto name = ask("名称");
                auto species = ask("物种");
                int age = number("年龄");
                auto gender = ask("性别");
                auto zone = ask("园区编号");
                confirm("添加 " + name + " 到 " + zone);
                if (!animals.addAnimal(category, name, species, age, gender, zone))
                    std::cout << animals.getLastError() << '\n';
                else
                    changed = true;
            } else if (choice == "2" || choice == "3" || choice == "4" || choice == "5") {
                auto id = ask("AnimalID");
                AnimalDetailInfo info;
                if (!animals.getAnimalDetailInfo(id, info)) {
                    std::cout << "动物不存在。\n";
                    continue;
                }
                show(info.basic);
                std::cout << "年龄：" << info.age << " 性别：" << info.gender << '\n';
                if (choice == "5")
                    continue;
                if (choice == "2" || choice == "3") {
                    auto name = info.basic.name;
                    auto gender = info.gender;
                    int age = info.age;
                    if (choice == "2") {
                        name = ask("新名称");
                        age = number("新年龄");
                        gender = ask("新性别");
                    }
                    auto zone = ask("新园区编号");
                    confirm("修改 " + id + " 的资料/园区");
                    changed = animals.updateAnimalInfo(id, name, age, gender, zone);
                } else {
                    auto code = ask("新状态 NORMAL/TREATMENT/ISOLATED/TRANSFERRED/DECEASED");
                    AnimalStatus status;
                    if (!animalStatusFromCode(code, status)) {
                        std::cout << "状态编码非法。\n";
                        continue;
                    }
                    confirm(id + "：" + animalStatusToCode(info.basic.status) + " → " + code);
                    changed = animals.changeAnimalStatus(id, status);
                }
                if (!changed)
                    std::cout << animals.getLastError() << '\n';
            } else if (choice == "6")
                list(animals.searchAnimals(ask("关键字（回车查全部）", true)));
            else if (choice == "7")
                list(animals.getAnimalsByZone(ask("园区编号")));
            else if (choice == "8")
                list(animals.searchAnimals(""));
            else if (choice == "9") {
                for (const auto &z : zones.getAllZones())
                    std::cout << z.zoneId << " | " << z.zoneName << " | " << z.description << '\n';
            } else if (choice == "A" || choice == "a") {
                auto id = ask("新ZoneID");
                auto name = ask("园区名称");
                auto desc = ask("描述（允许空）", true);
                confirm("添加园区 " + id + " " + name);
                changed = zones.addZone(Zone(id, name, desc));
                if (!changed)
                    std::cout << zones.getLastError() << '\n';
            } else
                std::cout << "不存在的选项。\n";
            if (changed) {
                if (storage.save())
                    std::cout << "操作成功，已保存。\n";
                else
                    std::cout << "内存已修改，保存失败，请重试保存：" << storage.getLastError()
                              << '\n';
            }
        } catch (const Navigate &n) {
            if (n.code != '0')
                return n.code;
            if (!inForm)
                return '0';
            std::cout << "已取消，未提交本次录入。\n";
        } catch (const std::invalid_argument &e) {
            std::cout << e.what() << '\n';
        }
    }
}
