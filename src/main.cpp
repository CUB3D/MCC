#include <iostream>
#include <fstream>
#include <string>
#include <nlohmann/json.hpp>
#include <cxxopts.hpp>
#include <sstream>
#include "mccState.h"
#include "mccToken.h"
#include "mccStateMachine.h"
#include "mccRegister.h"
#include "mccTokenStream.h"

struct mccConfig {
    std::string infile;
    std::string outfile;
    std::string archfile;
} mccGlobalConfig;

mccStateMachine initStateSystem() {
    mccStateMachine stateMachine;

    auto& identifier = stateMachine.addState("Identifier", "abcdefghijklmnopqrstuvwxyz"
                                              "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    auto& number = stateMachine.addState("Number", "1234567890xABCDEF");
    auto& whitespace = stateMachine.addState("Whitespace", " \n");
    auto& seperator = stateMachine.addState("Separator", ",");
    auto& commentLine = stateMachine.addState("Comment", ";");
    auto& assemblerCommand = stateMachine.addState("AssemblerCmd", ".");
    auto& startMem = stateMachine.addState("StartMem", "[");
    auto& endMem = stateMachine.addState("EndMem", "]");

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

    return stateMachine;
}

std::vector<mccToken> mccTokenise(std::istream& input) {
    auto stateMachine = initStateSystem();

    std::vector<mccToken> tokens;

    char c;
    input.read(&c, 1);
    mccState currentState = stateMachine.getStateFor(c);
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
                if(stateMachine.getState(possibleState).accepts(c)) {
                    currentState = stateMachine.getState(possibleState);
                }
            }

            // If we didn't find a new one, then error
            if(oldState == currentState) {
                printf("Error: invalid character %c at position %d\n", c, input.tellg());
                exit(-1);
            }

            // Generate new token
            auto& newToken = tokens.emplace_back(currentState);
            newToken.value += c;
        }
    }

    return tokens;
}

unsigned long int parseHexString(const std::string& str) {
    std::stringstream stream;
	stream << std::hex << str;
	unsigned long int x = 0;
	stream >> x;
	return x;
}

std::map<std::string, mccRegister> parseArchitecture() {
    using nlohmann::json;

    std::map<std::string, mccRegister> registersMap;

    json j;

    // Read in arch map
    std::ifstream i(mccGlobalConfig.archfile);
    i >> j;

    // Read all of the registers
    auto registers = j.at("Registers");
    for(auto& [name, value] : registers.items()) {

        unsigned long int readBit = parseHexString(value.value("ReadBit", ""));
        unsigned long int writeBit = parseHexString(value.value("WriteBit", ""));
        unsigned long int incrementBit = parseHexString(value.value("IncrementBit", ""));

        registersMap.insert(std::pair(name, mccRegister(name, readBit, writeBit, incrementBit)));
    }

    return registersMap;
}

std::string to_string(unsigned long int n) {
    std::stringstream ss;
    ss << std::hex << n;
    return ss.str();
}

void tokeniseStream(std::vector<mccToken> tokens) {
    std::vector<std::string> tempBuffer;

    auto registers = parseArchitecture();

    mccTokenStream ts(std::move(tokens));

    std::map<std::string, std::function<void(void)>> baseRecognisers;
    baseRecognisers["mov"] = [&]() {
        ts.consume(); // eat mov

        ts.consume_if("Whitespace");

        auto src = ts.consumeValue();

        ts.consume_if("Separator");
        ts.consume_if("Whitespace");

        auto dest = ts.consumeValue();

        auto regSRC = registers.at(src);
        auto regDest = registers.at(dest);

//        std::cout << "MOV " << src << ", " << dest << " = " << std::hex
//        << (regSRC.readBit | regDest.writeBit) << std::endl;

        unsigned long long opcode = (regSRC.readBit | regDest.writeBit);
        tempBuffer.push_back(to_string(opcode));
    };

    baseRecognisers["."] = [&]() {
        ts.consume();
        auto cmd = ts.consumeValue();
        ts.consume_if("Whitespace");

        if(cmd == "fill") {
            auto count = ts.consumeValue();

            std::stringstream ss;
            ss << std::hex << count.substr(2);
            int i = 0;
            ss >> i;

            unsigned long long opcode = 0;
            tempBuffer.push_back(std::to_string(i) + "*" + to_string(opcode));
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
        ts.consume_if("Whitespace");

        auto reg = ts.consumeValue();
        auto regReg = registers.at(reg);

//        std::cout << "INC " << reg << " = " << std::hex
//        << regReg.incrementBit << std::endl;

        unsigned long long opcode = (regReg.incrementBit);
        tempBuffer.push_back(to_string(opcode));
    };


    while(!ts.tokens.empty()) {
        ts.consume_if("Whitespace");
        auto startValue = ts.peekValue();

        if(std::find_if(baseRecognisers.begin(), baseRecognisers.end(), [&](auto a){return a.first == startValue;}) != baseRecognisers.end()) {
            baseRecognisers[startValue]();
        } else {
            std::cout << "Invalid token" << startValue << std::endl;
        }
    }

    std::ofstream out(mccGlobalConfig.outfile, std::ios::out);
    out << "v2.0 raw\n";

    int tmp = 0;
    while(!tempBuffer.empty()) {

        out << tempBuffer.front();
        tempBuffer.erase(std::find(tempBuffer.begin(), tempBuffer.end(), tempBuffer.front()));

        tmp++;

        if(tmp > 8) {
            tmp = 0;
            out << "\n";
        } else {
            out << " ";

        }
    }
}


int main(int argc, char** argv) {
    cxxopts::Options options("mcc", "The microcode compiler");
    options.add_options()
        ("i,input", "Input file", cxxopts::value<std::string>())
        ("o,output", "Output file", cxxopts::value<std::string>()->default_value("rom.bin"))
        ("a,arch", "Architecture definition file", cxxopts::value<std::string>())
    ;

    auto results = options.parse(argc, argv);

    mccGlobalConfig.infile = results["input"].as<std::string>();
    mccGlobalConfig.outfile = results["output"].as<std::string>();
    mccGlobalConfig.archfile = results["arch"].as<std::string>();

    std::ifstream input(mccGlobalConfig.infile);
    auto tokens = mccTokenise(input);
    tokeniseStream(tokens);

    return 0;
}