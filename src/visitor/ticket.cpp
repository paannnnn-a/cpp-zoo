#include "visitor/ticket.h"
using namespace std;

///注：此处的5000、2500、2000为分 以便于折扣的精确计算
Ticket::Ticket(Money initialprice) {
    price = initialprice;
    onsale = true;
}
Ticket::~Ticket() {
}
Money Ticket::getprice() const {
    return price;
}
bool Ticket::setprice(Money newprice) {
    if (newprice <= 0) {
        return false;
    }
    price = newprice;
    return true;
}
bool Ticket::getonsale() const {
    return onsale;
}
void Ticket::setonsale(bool value) {
    onsale = value;
}
bool Ticket::needqualification() const {
    return false;
}
AdultTicket::AdultTicket()
    : Ticket(5000) {
}
TicketType AdultTicket::gettype() const {
    return TicketType::adult;
}
string AdultTicket::getname() const {
    return "成人票";
}
string AdultTicket::geteligibility() const {
    return "成人游客";
}
DiscountTicket::DiscountTicket(Money initialprice)
    : Ticket(initialprice) {
}
bool DiscountTicket::needqualification() const {
    return true;
}
StudentTicket::StudentTicket()
    : DiscountTicket(2500) {
}
TicketType StudentTicket::gettype() const {
    return TicketType::student;
}
string StudentTicket::getname() const {
    return "学生票";
}
string StudentTicket::geteligibility() const {
    return "具有学生身份的游客";
}
ChildTicket::ChildTicket()
    : DiscountTicket(2000) {
}
TicketType ChildTicket::gettype() const {
    return TicketType::child;
}
string ChildTicket::getname() const {
    return "儿童票";
}
string ChildTicket::geteligibility() const {
    return "儿童游客";
}