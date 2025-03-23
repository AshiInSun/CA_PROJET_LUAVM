//
// Created by Shaimani on 21/03/2025.
//
#include "LuaInstructions.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <unordered_map>
#include "LuaInstructions.h"

// Implémentation du constructeur
LuaInstructions::LuaInstructions(uint8_t opcode, uint8_t A, uint16_t B, uint16_t C, uint32_t Bx, int32_t sBx)
        : opcode(opcode), A(A), B(B), C(C), Bx(Bx), sBx(sBx) {
    opcodeName = decodeInstr();
}

// Implémentation de decodeInstr()

std::string LuaInstructions::decodeInstr() {
    switch (opcode) {
        case 0: return "MOVE";
        case 1: return "LOADK";
        case 2: return "LOADBOOL";
        case 3: return "LOADNIL";
        case 4: return "GETUPVAL";
        case 5: return "GETGLOBAL";
        case 6: return "GETTABLE";
        case 7: return "SETGLOBAL";
        case 8: return "SETUPVAL";
        case 9: return "SETTABLE";
        case 10: return "NEWTABLE";
        case 11: return "SELF";
        case 12: return "ADD";
        case 13: return "SUB";
        case 14: return "MUL";
        case 15: return "DIV";
        case 16: return "MOD";
        case 17: return "POW";
        case 18: return "UNM";
        case 19: return "NOT";
        case 20: return "LEN";
        case 21: return "CONCAT";
        case 22: return "JMP";
        case 23: return "EQ";
        case 24: return "LT";
        case 25: return "LE";
        case 26: return "TEST";
        case 27: return "TESTSET";
        case 28: return "CALL";
        case 29: return "TAILCALL";
        case 30: return "RETURN";
        default: return "UNKNOWN OPCODE";
    }
}

