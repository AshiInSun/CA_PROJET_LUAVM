//
// Created by Shaimani on 21/03/2025.
//

#include "LuaChunk.h"
#include <iostream>

LuaChunk::LuaChunk()
        : name(""), first_line(0), last_line(0), upvalues(0),
          arguments(0), vararg(0), stack_size(0) {}

LuaChunk::LuaChunk(const std::string &name, int first_line, int last_line, uint8_t upvalues,
                   uint8_t arguments, uint8_t vararg, uint8_t stack_size)
        : name(name), first_line(first_line), last_line(last_line),
          upvalues(upvalues), arguments(arguments), vararg(vararg),
          stack_size(stack_size) {}

void LuaChunk::addInstruction(const LuaInstructions &instr) {
    instructions.push_back(instr);
}

void LuaChunk::addConstant(int index, const std::string &value) {
    constants[index] = value;
}

void LuaChunk::addPrototype(const LuaChunk &chunk) {
    prototypes.push_back(chunk);
}

void LuaChunk::printInfo() const {
    std::cout << "LuaChunk: " << name << "\n"
              << "First line: " << first_line << ", Last line: " << last_line << "\n"
              << "Upvalues: " << (int)upvalues << ", Arguments: " << (int)arguments
              << ", Vararg: " << (int)vararg << ", Stack Size: " << (int)stack_size << "\n"
              << "Instructions Count: " << instructions.size() << "\n"
              << "Constants Count: " << constants.size() << "\n"
              << "Prototypes Count: " << prototypes.size() << "\n";
}
