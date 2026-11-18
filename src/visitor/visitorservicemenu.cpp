#include "visitor/visitormenu.h"
#include "visitor/visitorutils.h"
#include "AnimalManager.h"
#include <iostream>
using namespace std;
char VisitorMenu::servicemenu() {
    while (session.getrole() == Role::visitor) {
        cout << "\n========== 游园与反馈服务 ==========\n";
        cout << "1. 浏览动物信息\n";
        cout << "2. 查看推荐路线\n";
        cout << "3. 提交建议\n";
        cout << "4. 提交游园评价\n";
        cout << "5. 查询我的反馈\n";
        cout << "6. 查看反馈详情\n";
        cout << "0. 返回上一级\n";
        cout << "M. 返回系统主菜单\n";
        cout << "Q. 保存并退出系统\n";
        cout << "请选择：";
        string choice;
        if (!getline(cin, choice)) {
            return 'Q';
        }
        if (choice == "0") {
            return '0';
        }
        if (choice == "M" || choice == "m") {
            return 'M';
        }
        if (choice == "Q" || choice == "q") {
            return 'Q';
        }
        if (choice == "1") {
            browseanimals();
        }
        else if (choice == "2") {
            showroutes();
        }
        else if (choice == "3") {
            submitsuggestion();
        }
        else if (choice == "4") {
            submitreview();
        }
        else if (choice == "5") {
            queryfeedback();
        }
        else if (choice == "6") {
            showfeedbackdetail();
        }
        else {
            cout << "无效选项，请重新输入\n";
        }
    }
    return '0';
}
///浏览动物信息
void VisitorMenu::browseanimals() {
    cout << "\n========== 浏览动物信息 ==========\n";
    string keyword;
    if (!readtext("请输入动物名称或物种关键词，留空查看全部",
        keyword, true)) {
        return;
    }
    vector<AnimalBasicInfo> result = animals.searchAnimals(keyword);
    int count = 0;
    for (size_t i = 0; i < result.size(); i++) {
        const AnimalBasicInfo& animal = result[i];
        if (animal.status == AnimalStatus::TRANSFERRED
            || animal.status == AnimalStatus::DECEASED) {
            continue;
        }
        showanimal(animal);
        count++;
    }
    if (count == 0) {
        cout << "没有符合条件的动物\n";
    }
    else {
        cout << "\n共找到" << count << "只动物\n";
    }
}
void VisitorMenu::showanimal(const AnimalBasicInfo& animal) const {
    cout << "\n动物编号：" << animal.animalId << "\n";
    cout << "动物名称：" << animal.name << "\n";
    cout << "物种：" << animal.species << "\n";
    cout << "所在园区：" << animal.zoneName << "\n";
    cout << "展出情况：";
    if (animal.onDisplay) {
        cout << "正在展出\n";
    }
    else {
        cout << "暂不展出\n";
    }
}
///查看推荐路线
void VisitorMenu::showroutes() {
    cout << "\n========== 推荐路线 ==========\n";
    string keyword;
    if (!readtext("请输入路线关键词，留空查看全部",
        keyword, true)) {
        return;
    }
    vector<Route> result = routes.getroutes(keyword);
    if (result.empty()) {
        cout << "没有符合条件的路线\n";
        return;
    }
    for (size_t i = 0; i < result.size(); i++) {
        cout << "\n路线编号：" << result[i].routeid << "\n";
        cout << "路线名称：" << result[i].name << "\n";
        cout << "路线介绍：" << result[i].description << "\n";
    }
    string routeid;
    if (!readtext("请输入要查看的路线编号", routeid)) {
        return;
    }
    RouteDetail detail;
    if (!routes.getroutedetail(routeid, detail)) {
        cout << routes.getlasterror() << "\n";
        return;
    }
    cout << "\n========== 路线详情 ==========\n";
    cout << "路线名称：" << detail.route.name << "\n";
    cout << "当前游览顺序：";
    for (size_t i = 0; i < detail.stops.size(); i++) {
        if (i != 0) {
            cout << " → ";
        }
        cout << detail.stops[i].zone.zoneName;
    }
    cout << "\n";
    // 按站点顺序显示园区和正在展出的动物
    for (size_t i = 0; i < detail.stops.size(); i++) {
        const RouteStop& stop = detail.stops[i];
        cout << "\n第" << i + 1 << "站：" << stop.zone.zoneName << "\n";
        cout << "园区介绍：" << stop.zone.description << "\n";
        for (size_t j = 0; j < stop.animals.size(); j++) {
            showanimal(stop.animals[j]);
        }
    }
    if (!detail.skippedzoneids.empty()) {
        cout << "\n以下园区不存在或暂无展出动物，本次已跳过：\n";
        for (size_t i = 0; i < detail.skippedzoneids.size(); i++) {
            cout << detail.skippedzoneids[i] << "\n";
        }
    }
}
///提交建议和游园评价
void VisitorMenu::submitsuggestion() {
    cout << "\n========== 提交建议 ==========\n";
    cout << "无需购票即可提交建议，内容请写在一行内，不要包含竖线|\n";
    string content;
    if (!readtext("请输入建议内容", content)) {
        return;
    }
    string feedbackid;
    bool success = feedback.submitsuggestion(
        session, content, feedbackid);
    finishoperation(session, "SUBMIT_SUGGESTION", feedbackid,
        success, feedback.getlasterror());
}
void VisitorMenu::submitreview() {
    cout << "\n========== 提交游园评价 ==========\n";
    cout << "只能评价本人实际检票入园的日期，同一天只能评价一次\n";
    string visitdate;
    if (!readtext("请输入游览日期，格式YYYY-MM-DD", visitdate)) {
        return;
    }
    int rating;
    if (!readint("请输入评分，1分最低，5分最高", 1, 5, rating)) {
        return;
    }
    cout << "评价内容请写在一行内，不要包含竖线|\n";
    string content;
    if (!readtext("请输入评价内容", content)) {
        return;
    }
    string feedbackid;
    bool success = feedback.submitreview(
        session, visitdate, rating, content, feedbackid);
    finishoperation(session, "SUBMIT_REVIEW", feedbackid,
        success, feedback.getlasterror());
}
///查询我的反馈
void VisitorMenu::queryfeedback() {
    cout << "\n========== 查询反馈 ==========\n";
    FeedbackQuery query;
    if (session.getrole() == Role::admin) {
        if (!readtext("游客编号，留空查询全部游客",
            query.visitorid, true)) {
            return;
        }
    }
    cout << "反馈种类：0.不限 1.建议 2.游园评价\n";
    int type;
    if (!readint("请选择反馈种类", 0, 2, type)) {
        return;
    }
    if (type != 0) {
        query.filtertype = true;
        if (type == 1) {
            query.type = FeedbackType::suggestion;
        }
        else {
            query.type = FeedbackType::review;
        }
    }
    cout << "处理状态：0.不限 1.待处理 2.处理中 3.已解决\n";
    int status;
    if (!readint("请选择处理状态", 0, 3, status)) {
        return;
    }
    if (status != 0) {
        query.filterstatus = true;
        if (status == 1) {
            query.status = FeedbackStatus::pending;
        }
        else if (status == 2) {
            query.status = FeedbackStatus::processing;
        }
        else {
            query.status = FeedbackStatus::resolved;
        }
    }
    // 建议没有对应的游览日期
    if (type != 1) {
        if (!readtext("游览日期，格式YYYY-MM-DD，留空不限",
            query.visitdate, true)) {
            return;
        }
    }
    vector<Feedback> result;
    if (!feedback.getfeedbacks(session, query, result)) {
        cout << feedback.getlasterror() << "\n";
        return;
    }
    if (result.empty()) {
        cout << "没有符合条件的反馈\n";
        return;
    }
    cout << "共找到" << result.size() << "条反馈\n";
    for (size_t i = 0; i < result.size(); i++) {
        const Feedback& item = result[i];
        cout << "\n反馈编号：" << item.feedbackid << "\n";
        cout << "游客编号：" << item.visitorid << "\n";
        cout << "反馈种类：" << feedbacktypename(item.type) << "\n";
        cout << "提交时间：" << item.createdat << "\n";
        cout << "处理状态：" << feedbackstatusname(item.status) << "\n";
        if (item.type == FeedbackType::review) {
            cout << "游览日期：" << item.visitdate << "\n";
            cout << "评分：" << item.rating << "分\n";
        }
        cout << "内容：" << item.content << "\n";
    }
}
///查看反馈详情
void VisitorMenu::showfeedbackdetail() {
    string feedbackid;
    if (!readtext("请输入反馈编号", feedbackid)) {
        return;
    }
    Feedback result;
    if (!feedback.getfeedbackdetail(session, feedbackid, result)) {
        cout << feedback.getlasterror() << "\n";
        return;
    }
    showfeedback(result);
}
void VisitorMenu::showfeedback(const Feedback& item) const {
    cout << "\n========== 反馈详情 ==========\n";
    cout << "反馈编号：" << item.feedbackid << "\n";
    cout << "游客编号：" << item.visitorid << "\n";
    cout << "反馈种类：" << feedbacktypename(item.type) << "\n";
    cout << "提交时间：" << item.createdat << "\n";
    if (item.type == FeedbackType::review) {
        cout << "游览日期：" << item.visitdate << "\n";
        cout << "评分：" << item.rating << "分\n";
    }
    cout << "内容：" << item.content << "\n";
    cout << "处理状态：" << feedbackstatusname(item.status) << "\n";
    if (item.reply.empty()) {
        cout << "管理员回复：暂无\n";
    }
    else {
        cout << "管理员回复：" << item.reply << "\n";
    }
    if (!item.operatorid.empty()) {
        cout << "处理人：" << item.operatorid << "\n";
        cout << "最近处理时间：" << item.handledat << "\n";
    }
}
