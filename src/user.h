#pragma once
#include <string>

/// @brief Created and managed by UserManager
class User
{
    private:
        std::string name;
        std::string rfid; 
        int spending;
        bool blocked;
    
    public:
        User(const std::string& user_name, std::string id, int amout, bool blocked = false);

        const std::string& getName() const { return name; };
        const std::string& getRFID() const { return rfid; };
        // const std::string& getBlocked() ;
        int getSpending() const { return spending; };
        bool isBlockedStr() const;

        void addSpending(int value);
        void setSpending(int value);

        bool isBlocked();

        std::string getPrintableData(bool print_id = false) const; 
};