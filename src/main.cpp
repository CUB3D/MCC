#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <map>
#include <functional>
#include "mccState.h"
#include "MccToken.h"

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
}

void initStateSystem() {
    auto& identifier = addState("Identifier", "abcdefghijklmnopqrstuvwxyz"
                                              "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    auto& number = addState("Number", "1234567890x");
    auto& whitespace = addState("Whitespace", " \n");
    auto& seperator = addState("Separator", ",");
    auto& commentLine = addState("Comment", ";");
    auto& assemblerCommand = addState("AssemblerCmd", ".");
    auto& startMem = addState("StartMem", "[");
    auto& endMem = addState("EndMem", "]");

    identifier += whitespace;
    identifier += seperator;
    identifier += commentLine;
    identifier += endMem;

    whitespace += identifier;
    whitespace += seperator;
    whitespace += commentLine;
    whitespace += number;
    whitespace += assemblerCommand;
    whitespace += startMem;

    seperator += whitespace;

    commentLine += whitespace;
    commentLine += identifier;

    assemblerCommand += whitespace;
    assemblerCommand += identifier;

    number += whitespace;
    number += endMem;

    startMem += number;
    startMem += identifier;

    endMem += seperator;
}

std::vector<mccToken> mccTokenise(std::istream& input) {
    std::vector<mccToken> tokens;

    char c;
    input.read(&c, 1);
    mccState currentState = getStateFor(c);
    tokens.emplace_back(currentState);
    tokens.back().value += c;

    while(input.read(&c, 1)) {

        if(currentState.accepts(c)) {
            // Get the current token
            auto &token = tokens.back();
            // Add current value to token
            token.value += c;
        } else {
            // Need to change state

            mccState oldState = currentState;

            // Find a state that accepts being transitioned to from this one
            for(auto&& possibleState : currentState.nextStates) {
                if(getState(possibleState).accepts(c)) {
                    currentState = getState(possibleState);
                }
            }

            // If we didn't find a new one, then error
            if(oldState == currentState) {
                printf("Error: invalid character %c at position %d\n", c, input.tellg());
                exit(-1);
            }

            printf("State transition: moved to %s\n", currentState.name.c_str());

            // Generate new token
            auto& newToken = tokens.emplace_back(currentState);
            newToken.value += c;
        }
    }

    return tokens;
}

class tokenStream {
public:
    std::vector<mccToken> tokens;

    tokenStream(std::vector<mccToken> vector)  : tokens(vector) {}

    mccToken& peek() {
        return tokens.front();
    }

    std::string peekValue() {
        return peek().value;
    }

    mccState& peekType() {
        return peek().tokenType;
    }



    void consume() {
        tokens.erase(std::find(tokens.begin(), tokens.end(), tokens.front()));
    }

    std::string consumeValue() {
        std::string s = peekValue();
        consume();
        return s;
    }

    void consume_if(const mccState state) {
        if(peekType() == state) {
            consume();
        }
    }
};

class mccRegister {
public:
    std::string name;
    unsigned long int readBit;
    unsigned long int writeBit;
    unsigned long incrementBit;

    mccRegister(const std::string &name, unsigned long int read, unsigned long int write=0,
                unsigned long int increment=0) :
                name(name),
                readBit(read),
                writeBit(write),
                incrementBit(increment) {}
};

void tokeniseStream(std::vector<mccToken> tokens) {
    // TODO: move
    std::map<std::string, mccRegister> registers;
    registers.insert(std::pair("ML",   mccRegister("ML", 0x00000008, 0x00000004)));
    registers.insert(std::pair("PC",   mccRegister("PC", 0x00000001, 0, 0x00000002)));
    registers.insert(std::pair("IR",   mccRegister("IR", 0x00001000, 0x00000040)));
    registers.insert(std::pair("RAM", mccRegister("RAM", 0x00000010, 0x00000020)));
    registers.insert(std::pair("A",     mccRegister("A", 0x00000200, 0x00000080)));
    registers.insert(std::pair("B",     mccRegister("B", 0x00000000, 0x00000004)));
    registers.insert(std::pair("ALU",     mccRegister("ALU", 0x00000000, 0x00002000)));

    tokenStream ts(tokens);

    std::map<std::string, std::function<void(void)>> baseRecognisers;
    baseRecognisers["mov"] = [&]() {
        ts.consume(); // eat mov

        ts.consume_if(getState("Whitespace"));

        auto src = ts.consumeValue();

        ts.consume_if(getState("Separator"));
        ts.consume_if(getState("Whitespace"));

        auto dest = ts.consumeValue();

        // std::cout << "MOV " << "SRC: " << src << " DEST: " << dest << "\n";

        auto regSRC = registers.at(src);
        auto regDest = registers.at(dest);


        std::cout << "MOV " << src << ", " << dest << " = " << std::hex
        << (regSRC.readBit | regDest.writeBit) << std::endl;
    };

    baseRecognisers["."] = [&]() {
        ts.consume();
        auto cmd = ts.consumeValue();
        ts.consume_if(getState("Whitespace"));

        if(cmd == "instructionWidth") {
            auto width = ts.consumeValue();

            std::cout << "IW: " << width << std::endl;
        }

        if(cmd == "fill") {
            auto count = ts.consumeValue();
        }
    };

    baseRecognisers[";"] = [&]() {
        do {
            auto tmp = ts.consumeValue();

            if(tmp.find('\n') != std::string::npos) {
                return;
            }

        } while(true);
    };

    baseRecognisers["inc"] = [&]() {
        ts.consume();
        ts.consume_if(getState("Whitespace"));

        auto reg = ts.consumeValue();
        auto regReg = registers.at(reg);

        std::cout << "INC " << reg << " = " << std::hex
        << regReg.incrementBit << std::endl;
    };


    while(!ts.tokens.empty()) {
        ts.consume_if(getState("Whitespace"));
        auto startValue = ts.peekValue();

        if(std::find_if(baseRecognisers.begin(), baseRecognisers.end(), [&](auto a){return a.first == startValue;}) != baseRecognisers.end()) {
            baseRecognisers[startValue]();
        } else {
            std::cout << "Invalid token" << startValue << std::endl;
        }
    }
}


int main() {
    initStateSystem();

    std::ifstream input("./test.mcc");
    auto tokens = mccTokenise(input);
    tokeniseStream(tokens);


    return 0;
}