//
// Created by cub3d on 07/01/19.
//

#ifndef MCC_MCCTOKENSTREAM_H
#define MCC_MCCTOKENSTREAM_H

#include "mccToken.h"
#include "mccVectorStream.h"
#include <string>

class mccTokenStream : public mccVectorStream<mccToken> {
public:
    explicit mccTokenStream(std::vector<mccToken> tokens);

    std::string peekValue() {
        return peek().value;
    }

    mccState& peekType() {
        return peek().tokenType;
    }

    std::string consumeValue() {
        std::string s = peekValue();
        consume();
        return s;
    }
    void consume_if(const std::string& name) {
        if(peekType().name == name) {
            consume();
        }
    }
};


#endif //MCC_MCCTOKENSTREAM_H
