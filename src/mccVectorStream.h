//
// Created by cub3d on 07/01/19.
//

#ifndef MCC_MCCVECTORSTREAM_H
#define MCC_MCCVECTORSTREAM_H

#include <vector>
#include <string>
#include "mccToken.h"
#include "mccState.h"

template<typename T>
class mccVectorStream {
public:
    std::vector<T> tokens;

    explicit mccVectorStream(std::vector<T> vector)  : tokens(vector) {}

    T& peek() {
        return tokens.front();
    }


    void consume() {
        tokens.erase(std::find(tokens.begin(), tokens.end(), tokens.front()));
    }
};


#endif //MCC_MCCVECTORSTREAM_H
