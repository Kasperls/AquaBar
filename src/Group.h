#pragma once
#include <string>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

enum class Group{
    NONE, 
    FG,
    ITK,
    SM,
    FK,
    REGI,
    VK,
    DG,
    KSG,
    ARK,
    GU,
    SIT,
    MG,
    KU,
    LOK,
    KS,
    SKP
};

inline std::string getTextFromGroup(Group group) {
    static const std::unordered_map<Group, std::string> table = {
        {Group::FG, "FG",    },
        {Group::ITK, "ITK",  },
        {Group::SM, "SM",    },
        {Group::FK, "FK",    },
        {Group::REGI, "REGI",},
        {Group::VK, "VK",    },
        {Group::DG, "DG",    },
        {Group::KSG, "KSG",  },
        {Group::ARK, "ARK",  },
        {Group::GU, "GU",    },
        {Group::SIT, "SIT",  },
        {Group::MG, "MG",    },
        {Group::KU, "KU",    },
        {Group::LOK, "LOK",  },
        {Group::KS, "KS",    },
        {Group::SKP, "SKP",  }
    };
    auto it = table.find(group);
    return (it != table.end()) ? it->second : "NONE";
}

inline Group createGroupFromString(std::string str) {
    std::string upper;
    std::cout << str << std::endl;
    for (char& c : str) {
        if (!std::isalpha(c)) {
            throw std::invalid_argument("INVALID GROUP NAME");
        }
        upper.push_back(std::toupper(c));
    }

    static const std::unordered_map<std::string, Group> table = {
        {"FG",  Group::FG   },
        {"ITK", Group::ITK  },
        {"SM",  Group::SM   },
        {"FK",  Group::FK   },
        {"REGI",Group::REGI },
        {"VK",  Group::VK   },
        {"DG",  Group::DG   },
        {"KSG", Group::KSG  },
        {"ARK", Group::ARK  },
        {"GU",  Group::GU   },
        {"SIT", Group::SIT  },
        {"MG",  Group::MG   },
        {"KU",  Group::KU   },
        {"LOK", Group::LOK  },
        {"KS",  Group::KS   },
        {"SKP", Group::SKP  }
    };

    auto it = table.find(str);
    return (it != table.end()) ? it->second : Group::NONE;
}