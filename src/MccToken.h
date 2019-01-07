//
// Created by cub3d on 06/01/19.
//

#ifndef MCC_MCCTOKEN_H
#define MCC_MCCTOKEN_H

#include <string>
#include "mccState.h"

class mccToken {
public:
    std::string value;
    mccState tokenType;

    mccToken(mccState type);

    bool operator==(const mccToken& other);

    virtual ~mccToken() = default;
};


#endif //MCC_MCCTOKEN_H
