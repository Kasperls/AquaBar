#include "userManager.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

UserManager::UserManager(const std::string& path_to_data) {
    std::ifstream data_file(path_to_data);

    if (!data_file.is_open()) {
        std::cout << "Failed to open file!" << std::endl;
        return;
    }

    std::cout << "File opened successfully" << std::endl;
    std::cout << "Path: " << path_to_data << std::endl;

    int lineCount = 0;
    std::string line;
    while (std::getline(data_file, line)) {
        lineCount++;
    }

    data_file.clear();
    data_file.seekg(0);
    user_vector.reserve(lineCount);
    
    while (std::getline(data_file, line)) {
        // "Kasper,3481191757,300,0"
        std::stringstream ss(line);
        std::string element;
        
        std::vector<std::string> raw_line_vector;
        while (std::getline(ss, element, ',')) {
            raw_line_vector.push_back(element);
        }

        user_vector.push_back(User{
            raw_line_vector[0],
            raw_line_vector[1],
            std::stoi(raw_line_vector[2]),
            std::stoi(raw_line_vector[3]) != 0
        });

    }
    full_path = std::filesystem::absolute(path_to_data).string();

}

// Function for reloading the UserManager when a website update happends.
/// @brief NULLS ALL THE AMOUNT/VALUES FIELD
void UserManager::reloadUserManager() {
    std::ifstream data_file(full_path);

    if (!data_file.is_open()) {
        std::cout << "Failed to open file!" << std::endl;
        return;
    }
    int lineCount = 0;
    std::string line;
    while (std::getline(data_file, line)) {
        lineCount++;
    }

    data_file.clear();
    data_file.seekg(0);
    user_vector.clear();
    user_vector.reserve(lineCount);
    
    while (std::getline(data_file, line)) {
        // "Kasper,3481191757,300,0"
        std::stringstream ss(line);
        std::string element;
        
        std::vector<std::string> raw_line_vector;
        while (std::getline(ss, element, ',')) {
            raw_line_vector.push_back(element);
        }

        user_vector.push_back(User{
            raw_line_vector[0],
            raw_line_vector[1],
            0,
            std::stoi(raw_line_vector[3]) != 0
        });
    }
}

std::vector<User>& UserManager::getUsers() {
    return user_vector;
}

const std::vector<User>& UserManager::getUsers() const {
    return user_vector;
}

void UserManager::printUsers() {
    for (User& user : user_vector) {
        std::cout << user.getName() << " : " << user.getSpending() << std::endl;
    };
}

/// @brief Saves UserManager data to csv
/// @param reset_value if the user values should be reset to 0
void UserManager::saveData(bool reset_value) {
    std::ofstream file(full_path);
    if (reset_value) {
        for (User& user : user_vector) {
            user.setSpending(0);
            file << user.getName() << "," 
            << user.getRFID() << "," 
            << "0" << ","
            << "0" << "\n";
        }
    } else {
        for (const User& user : user_vector) {
            file << user.getName() << "," 
            << user.getRFID() << "," 
            << user.getSpending() << ","
            << user.isBlockedStr() << "\n";
        }
    }
    file.close();
}

User& UserManager::getUser(const std::string& rfid) {
    for (User& i_user : getUsers()) {
        if (i_user.getRFID() == rfid) {
            return i_user;
        }
    }

    throw std::runtime_error("User not found: " + rfid);
}

const std::string& UserManager::getPath() const {
    return full_path;
}

// void UserManager::writeUserData(unsigned int index) {

// }