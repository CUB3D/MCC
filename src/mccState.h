//
// Created by cub3d on 06/01/19.
//

#ifndef MCC_MCCSTATE_H
#define MCC_MCCSTATE_H

#include <string>
#include <vector>
#include <algorithm>

class mccState {
public:
    std::string name;
    std::string condition;
    std::vector<std::string> nextStates;

    mccState(std::string name, std::string condition);

    bool accepts(const char& c);

    bool operator==(const mccState& other);

    void operator+=(const mccState& other);
};

#endif //MCC_MCCSTATE_H
