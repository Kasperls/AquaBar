#include "user.h"


User::User(const std::string& user_name, std::string id, int amout, bool blocked, std::string unparsed_group)
    : name(user_name), rfid(id), spending(amout), blocked(blocked) {
        group = createGroupFromString(unparsed_group);
};

void User::addSpending(int value) {
    spending += value;
}

void User::setSpending(int value) {
    spending = value;
}

bool User::isBlocked() const {
    return blocked;
}

bool User::isBlockedStr() const {
    return blocked;
};

// const std::string& User::getBlocked() { 
//     return std::to_string(blocked); 
// };

std::string User::getPrintableData(bool print_id) const {
    std::string return_text = "Name: ";
    return_text += name;
    return_text += ", Spending: ";
    return_text += std::to_string(spending);
    if (print_id) {
        return_text += ", RFID: ";
        return_text += rfid;
        return_text += ", Svart: ";
        return_text += blocked;
        return_text += getTextFromGroup(group);
    }
    
    return return_text;
}