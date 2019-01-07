//
// Created by cub3d on 06/01/19.
//

#include "MccToken.h"

mccToken::mccToken(mccState type) : tokenType(type) {}

bool mccToken::operator==(const mccToken& other) {
    return other.value == this->value;
}
