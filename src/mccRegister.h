//
// Created by cub3d on 07/01/19.
//

#ifndef MCC_MCCREGISTER_H
#define MCC_MCCREGISTER_H

#include <string>

class mccRegister {
public:
    std::string name;
    unsigned long int readBit;
    unsigned long int writeBit;
    unsigned long incrementBit;

    mccRegister(const std::string &name, unsigned long int read, unsigned long int write=0,
                unsigned long int increment=0);
};


#endif //MCC_MCCREGISTER_H
