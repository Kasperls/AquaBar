#include "userManager.h"
#include <iostream>
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
        // "Kasper,3481191757,300"
        std::stringstream ss(line);
        std::string element;
        
        std::vector<std::string> raw_line_vector;
        while (std::getline(ss, element, ',')) {
            raw_line_vector.push_back(element);
        }

        user_vector.push_back(User{
            raw_line_vector[0],
            raw_line_vector[1],
            std::stoi(raw_line_vector[2])
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
        std::cout << user.getName() << std::endl;
    };
}

void UserManager::saveData() {
    std::ofstream file(path_to_data);
    for (const User& user : user_vector) {
        file << user.getName() << "," 
             << user.getRFID() << "," 
             << user.getSpending() << "\n";
    }
    file.close();
}

// void UserManager::writeUserData(unsigned int index) {

// }