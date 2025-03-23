//
// Created by Shaimani on 21/03/2025.
//

#ifndef CA_PROJET_LUAVM_LUACHUNK_H
#define CA_PROJET_LUAVM_LUACHUNK_H


#include <string>
#include <vector>
#include <unordered_map>
#include "LuaInstructions.h"

class LuaChunk {
public:
    std::string name;
    int first_line;
    int last_line;
    uint8_t upvalues;
    uint8_t arguments;
    uint8_t vararg;
    uint8_t stack_size;
    std::vector<LuaInstructions> instructions;
    std::unordered_map<int, std::string> constants;
    std::vector<LuaChunk> prototypes;

    LuaChunk();
    LuaChunk(const std::string &name, int first_line, int last_line, uint8_t upvalues,
             uint8_t arguments, uint8_t vararg, uint8_t stack_size);

    void addInstruction(const LuaInstructions &instr);
    void addConstant(int index, const std::string &value);
    void addPrototype(const LuaChunk &chunk);
    void printInfo() const;

    int maxRegisters;
};
#endif //CA_PROJET_LUAVM_LUACHUNK_H
