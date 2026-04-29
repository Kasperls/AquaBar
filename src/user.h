#pragma once
#include <string>
#include "Group.h"

/// @brief Created and managed by UserManager
class User
{
    private:
        std::string name;
        std::string rfid; 
        int spending;
        bool blocked;
        Group group;
    
    public:
        User(const std::string& user_name, std::string id, int amout, std::string unparsed_group, bool blocked = false);

        const std::string& getName() const { return name; };
        const std::string& getRFID() const { return rfid; };
        // const std::string& getBlocked() ;
        int getSpending() const { return spending; };
        bool isBlockedStr() const;

        void addSpending(int value);
        void setSpending(int value);

        Group get_group() const { return group; };

        bool isBlocked() const;

        std::string getPrintableData(bool print_id = false) const; 
};