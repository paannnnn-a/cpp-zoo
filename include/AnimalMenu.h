#pragma once
#include "AStorage.h"
/**
 * @brief 实现动物资料、园区和状态的交互操作入口。
 * @note 只通过Manager读写业务，通过AStorage保存；不直接操作数据文件。
 */
/**
 * @brief 无继承关系的动物菜单；animals、zones、storage 为协作对象引用。构造函数绑定引用，run 完成输入、查询、确认修改及保存。
 */
class AnimalMenu {
  public:
    /**
     * @brief 构造AnimalMenu并绑定协作对象。
     * @param a [in,out] 共享动物管理器引用。
     * @param z [in,out] 园区管理器引用。
     * @param s [in,out] 统一保存协调器引用。
     * @note 构造函数无返回值；被引用对象必须覆盖本对象的整个使用期间。
     * @details 通过初始化列表初始化成员或绑定协作对象；实体构造复用字段校验，派生动物委托 Animal 构造。
 */
    AnimalMenu(AnimalManager &a, ZoneManager &z, AStorage &s) : animals(a), zones(z), storage(s) {}
    /**
     * @brief 循环显示菜单、校验输入、执行确认后的操作并自动保存。
     * @note 参数：无。
     * @return '0'返回上级；'M'返回主菜单；'Q'请求统一退出。
     * @note 录入中的0仅取消当前操作；EOF按Q传播；保存失败保留内存并提示重试。
     * @details 循环显示选项并读取输入；查询直接显示，修改经确认后调用管理器并保存；捕获导航和字段异常。
 */
    char run();

  private:
    AnimalManager &animals;
    ZoneManager &zones;
    AStorage &storage;
};
