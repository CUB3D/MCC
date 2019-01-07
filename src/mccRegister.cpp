//
// Created by cub3d on 07/01/19.
//

#include "mccRegister.h"

mccRegister::mccRegister(const std::string &name, unsigned long int read, unsigned long write,
                         unsigned long increment) :
        name(name),
        readBit(read),
        writeBit(write),
        incrementBit(increment) {}
