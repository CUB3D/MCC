//
// Created by cub3d on 06/01/19.
//

#include "mccState.h"

mccState::mccState(std::string name, std::string condition) : name(name), condition(condition) {}

bool mccState::accepts(const char &c) {
    return std::find(condition.begin(), condition.end(), c) != condition.end();
}

bool mccState::operator==(const mccState &other) {
    return other.name == this->name;
}

void mccState::operator+=(const mccState &other) {
    this->nextStates.push_back(other.name);
}
