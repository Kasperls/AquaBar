#pragma once
#include <vector>
#include "user.h"

// UserManager reads from a file and constructs users 
// UserManager is the only class that reads or writes 
// to the underlying csv file

class UserManager
{
    private:
        std::vector<User> user_vector;
        std::string path_to_data;
    
    
    public:
        UserManager(const std::string& path_to_data);

        std::vector<User>& getUsers();
        const std::vector<User>& getUsers() const;

        User& getUser(const std::string& rfid);

        void printUsers();

        void saveData();

        const std::string& getPath() const;

        // void writeUserData(unsigned int index);
};