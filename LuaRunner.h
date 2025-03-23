//
// Created by Shaimani on 21/03/2025.
//

#ifndef CA_PROJET_LUAVM_LUARUNNER_H
#define CA_PROJET_LUAVM_LUARUNNER_H

#include "LuaChunk.h"
#include "LuaInstructions.h"
#include <vector>
#include <iostream>
#include <functional>
#include <variant>

class LuaRunner {
private:
    LuaChunk chunk;  // Le chunk contenant le bytecode
    int pc;  // Compteur de programme (instruction actuelle)
    std::vector<intptr_t> registers;  // Registres pour stocker les valeurs
    std::unordered_map<std::string, std::variant<std::function<void(LuaRunner&, int, int)>, LuaChunk>> globals;
    LuaChunk* currentChunk; // Chunk en cours d'exécution


    // fonctions globales

public:
    // Constructeur
    LuaRunner(LuaChunk chunk);

    // Méthode principale pour exécuter le bytecode
    void run();
    void luaPrint(LuaRunner& vm, int A, int B);
    void executeFunction(LuaChunk& chunk, int A, int numArgs);

private:
    // Exécute une instruction spécifique
    void executeInstruction(const LuaInstructions& instr);
};

#endif // CA_PROJET_LUAVM_LUARUNNER_H
