#ifndef CA_PROJET_LUAVM_LUA_INSTRUCTIONS_H
#define CA_PROJET_LUAVM_LUA_INSTRUCTIONS_H

#include <cstdint>
#include <string>
#include <unordered_map>

class LuaInstructions {
public:
    uint8_t opcode;
    uint8_t A;
    uint16_t B;
    uint16_t C;
    uint32_t Bx;
    int32_t sBx;
    std::string opcodeName;

    LuaInstructions(uint8_t opcode, uint8_t A, uint16_t B, uint16_t C, uint32_t Bx, int32_t sBx);

    std::string decodeInstr();
};

#endif // LUA_INSTRUCTIONS_H
