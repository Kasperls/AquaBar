#include "user.h"


User::User(const std::string& user_name, std::string id, int amout)
    : name(user_name), rfid(id), spending(amout) {
};

void User::addSpending(int value) {
    spending += value;
}

void User::setSpending(int value) {
    spending = value;
}

std::string User::getPrintableData(bool print_id) const {
    std::string return_text = "Name: ";
    return_text += name;
    return_text += ", Spending: ";
    return_text += std::to_string(spending);
    if (print_id) {
        return_text += ", RFID: ";
        return_text += rfid;
    }
    
    return return_text;
}