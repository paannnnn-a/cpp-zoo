#pragma once
#include "AStorage.h"
/**
 * @brief 实现医疗登记、历史查询及完整动物档案的交互入口。
 * @note 收集并确认输入后才提交；疾病和康复状态联动由HealthManager处理。
 */
class HealthMenu {
  public:
    /**
     * @brief 构造HealthMenu并绑定协作对象。
     * @param a [in,out] 共享动物管理器引用。
     * @param h [in,out] 健康管理器引用。
     * @param s [in,out] 统一保存协调器引用。
     * @note 构造函数无返回值；被引用对象必须覆盖本对象的整个使用期间。
     */
    HealthMenu(AnimalManager &a, HealthManager &h, AStorage &s)
        : animals(a), health(h), storage(s) {}
    /**
     * @brief 循环显示菜单、校验输入、执行确认后的操作并自动保存。
     * 参数：无。
     * @return '0'返回上级；'M'返回主菜单；'Q'请求统一退出。
     * @note 录入中的0仅取消当前操作；EOF按Q传播；保存失败保留内存并提示重试。
     */
    char run();

  private:
    AnimalManager &animals;
    HealthManager &health;
    AStorage &storage;
};
