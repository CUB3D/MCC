//
// Created by cub3d on 07/01/19.
//

#ifndef MCC_MCCSTATEMACHINE_H
#define MCC_MCCSTATEMACHINE_H

#include <string>
#include <map>
#include "mccState.h"

class mccStateMachine {
public:
    std::map<std::string, mccState> stateTransitions;

    mccState& addState(const std::string& name, const std::string& condition) {
        stateTransitions.insert(std::pair(name, mccState(name, condition)));
        return stateTransitions.at(name);
    }

    mccState& getState(const std::string& name) { //Temp
        return stateTransitions.at(name);
    }

    mccState& getStateFor(char& c) {
        for(auto&& s : stateTransitions) {
            if(s.second.accepts(c))
                return getState(s.first);
        }

        throw std::exception();
    }
};


#endif //MCC_MCCSTATEMACHINE_H
