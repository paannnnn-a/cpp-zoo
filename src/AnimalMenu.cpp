#include "AnimalMenu.h"
#include "ConsoleInput.h"
using namespace zoo_ui;
/**
 * @brief 循环处理查询和录入；业务修改成功后立即调用 AStorage 尝试保存。
 * @return '0'返回上级；'M'返回主菜单；'Q'请求统一退出。
 * @note 录入中的0仅取消当前操作；EOF按Q传播；保存失败保留内存并提示重试。
 */
char AnimalMenu::run() {
    for (;;) {
        std::cout << "\n=== 动物管理 ===\n"
                     "1. 添加\n"
                     "2. 修改资料\n"
                     "3. 移动园区\n"
                     "4. 修改状态\n"
                     "5. 编号查询\n"
                     "6. 模糊查询\n"
                     "7. 园区查询\n"
                     "8. 全部档案（可直接按回车）\n"
                     "9. 园区列表\n"
                     "A. 添加园区\n";
        bool inForm = false;
        try {
            auto choice = ask("请选择", true);
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
            else if (choice == "8" || choice.empty())
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
