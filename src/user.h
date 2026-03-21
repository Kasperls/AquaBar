#pragma once
#include <string>

/// @brief Created and managed by UserManager
class User
{
    private:
        std::string name;
        std::string rfid; 
        int spending;
    
    public:
        User(const std::string& user_name, std::string id, int amout);

        const std::string& getName() const { return name; };
        const std::string& getRFID() const { return rfid; };
        int getSpending() const { return spending; };

        void addSpending(int value);
        void setSpending(int value);

        std::string getPrintableData(bool print_id = false) const; 
};