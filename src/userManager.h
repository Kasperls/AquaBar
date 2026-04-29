#pragma once
#include <vector>
#include "user.h"
#include <map>

// UserManager reads from a file and constructs users 
// UserManager is the only class that reads or writes 
// to the underlying csv file

class UserManager
{
    private:
        std::vector<User> user_vector;
        std::string full_path;
        std::map<std::string, std::vector<User>> group_vector;
    
    
    public:
        UserManager(const std::string& path_to_data);
        void reloadUserManager();

        std::vector<User>& getUsers();
        const std::vector<User>& getUsers() const;

        User& getUser(const std::string& rfid);

        void printUsers();

        void saveData(bool reset_value=false);

        const std::string& getPath() const;

};