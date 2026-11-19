#include "HealthMenu.h"
#include "ConsoleInput.h"
using namespace zoo_ui;
/**
 * @brief 循环显示菜单、校验输入、执行确认后的操作并自动保存。
 * @note 参数：无。
 * @return '0'返回上级；'M'返回主菜单；'Q'请求统一退出。
 * @note 录入中的0仅取消当前操作；EOF按Q传播；保存失败保留内存并提示重试。
 * @details 循环显示选项并读取输入；查询直接显示，修改经确认后调用管理器并保存；捕获导航和字段异常。
 */
char HealthMenu::run() {
    const auto showProfile = [&](const AnimalDetailInfo &info) {
        show(info.basic);
        std::cout << "年龄：" << info.age << " 性别：" << info.gender << '\n';
        medical(health.getMedicalRecordsByAnimal(info.basic.animalId));
    };
    for (;;) {
        std::cout << "\n=== 健康管理 ===\n"
                     "1. 体检\n"
                     "2. 疾病\n"
                     "3. 治疗\n"
                     "4. 康复\n"
                     "5. 医疗历史\n"
                     "6. 完整档案\n"
                     "回车. 查看全部健康档案\n";
        bool inForm = false;
        try {
            auto choice = ask("请选择", true);
            if (choice.empty()) {
                const auto all = animals.searchAnimals("");
                if (all.empty())
                    std::cout << "暂无动物档案。\n";
                for (const auto &animal : all) {
                    AnimalDetailInfo info;
                    if (animals.getAnimalDetailInfo(animal.animalId, info))
                        showProfile(info);
                }
                continue;
            }
            inForm = true;
            if (choice != "1" && choice != "2" && choice != "3" && choice != "4" && choice != "5" &&
                choice != "6") {
                std::cout << "不存在的选项。\n";
                continue;
            }
            auto id = ask("AnimalID");
            AnimalDetailInfo info;
            if (!animals.getAnimalDetailInfo(id, info)) {
                std::cout << "动物不存在。\n";
                continue;
            }
            if (choice == "5" || choice == "6") {
                if (choice == "6")
                    showProfile(info);
                else
                    medical(health.getMedicalRecordsByAnimal(id));
                continue;
            }
            auto date = ask("日期 YYYY-MM-DD");
            auto vet = ask("兽医编号");
            auto desc = ask("记录描述");
            const auto rid = health.generateRecordId();
            bool ok = false;
            std::optional<AnimalStatus> diseaseStatus;
            bool restore = false;
            if (choice == "2") {
                auto code = ask("1. 仅登记\n2. 改为治疗中\n3. 改为隔离\n请选择");
                if (code == "2")
                    diseaseStatus = AnimalStatus::TREATMENT;
                else if (code == "3")
                    diseaseStatus = AnimalStatus::ISOLATED;
                else if (code != "1") {
                    std::cout << "选项非法，未保存。\n";
                    continue;
                }
            }
            if (choice == "4") {
                auto code = ask("1. 记录并恢复正常\n2. 只记录不改状态\n请选择");
                if (code != "1" && code != "2") {
                    std::cout << "选项非法，未保存。\n";
                    continue;
                }
                restore = code == "1";
            }
            auto next = info.basic.status;
            if (diseaseStatus)
                next = *diseaseStatus;
            if (restore)
                next = AnimalStatus::NORMAL;
            confirm("为 " + id + " 添加记录 " + rid + "；状态 " +
                    animalStatusToCode(info.basic.status) + " → " + animalStatusToCode(next));
            if (choice == "1")
                ok = health.addHealthCheck(HealthCheckRecord(rid, id, date, vet, desc));
            if (choice == "2")
                ok =
                    health.addDiseaseRecord(DiseaseRecord(rid, id, date, vet, desc), diseaseStatus);
            if (choice == "3")
                ok = health.addTreatmentRecord(TreatmentRecord(rid, id, date, vet, desc));
            if (choice == "4")
                ok = health.addRecoveryRecord(RecoveryRecord(rid, id, date, vet, desc), restore);
            if (!ok)
                std::cout << health.getLastError() << '\n';
            else if (storage.save())
                std::cout << "记录及状态已保存。\n";
            else
                std::cout << "内存已修改，保存失败，请重试：" << storage.getLastError() << '\n';
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
