//
// Created by Shaimani on 21/03/2025.
//

#include "LuaRunner.h"
#include "LuaInstructions.h"
#include <iostream>
#include <functional>


LuaRunner::LuaRunner(LuaChunk chunk) {
    this->chunk = chunk;
    this->pc = 0;
    this->registers = std::vector<intptr_t>(chunk.stack_size, 0);
    //abracadabra weird things
    globals["print"] = [](LuaRunner& vm, int A, int B) {
        vm.luaPrint(vm, A, B);
    };


}

void LuaRunner::luaPrint(LuaRunner& vm, int A, int B) {
    std::cout << "[PRINT] ";
    for (int i = 1; i <= B; i++) {
        int reg = vm.registers[A + i];
        if (vm.chunk.constants.count(reg)) {
            std::cout << vm.chunk.constants[reg];
        } else {
            std::cout << reg;
        }
        if (i < B) std::cout << " ";
    }
    std::cout << std::endl;
}

void LuaRunner::executeFunction(LuaChunk& chunk, int A, int numArgs) {
    std::cout << "[EXEC] Démarrage de la fonction '" << chunk.name << "' avec " << numArgs << " arguments.\n";

    // Sauvegarde de l'état actuel (registre, programme, etc.)
    std::vector<intptr_t> savedRegisters = registers;
    int savedPC = pc;
    LuaChunk* savedChunk = currentChunk;

    // Prépare la nouvelle exécution
    currentChunk = &chunk;
    registers.resize(chunk.maxRegisters, 0);
    pc = 0;

    // Copier les arguments dans les registres de la fonction
    for (int i = 0; i < numArgs; i++) {
        registers[i] = savedRegisters[A + i + 1]; // Arguments à partir de R(A+1)
    }

    // Exécution du chunk
    run(); // Suppose que `run()` exécute `currentChunk`

    // Restaurer l'ancien état après l'exécution
    registers = savedRegisters;
    pc = savedPC;
    currentChunk = savedChunk;
}



void LuaRunner::executeInstruction(const LuaInstructions &instr) {
    switch (instr.opcode) {
        case 0: // MOVE
            registers[instr.A] = registers[instr.B];
            break;

        case 1: // LOADK
            registers[instr.A] = std::stoi(chunk.constants[instr.Bx]);
            std::cout << "LOADK : " << static_cast<int>(instr.A) << " <-" << std::stoi(chunk.constants[instr.Bx]) << std::endl;
            break;

        case 2: // LOADBOOL
            registers[instr.A] = instr.B;
            if (instr.C) pc++; // Saut de l’instruction suivante si C != 0
            break;

        case 3: // LOADNIL
            for (int i = instr.A; i <= instr.B; i++) {
                registers[i] = 0;
            }
            break;

        case 5: { // GETGLOBAL
            int A = instr.A;
            int Bx = instr.Bx;

            if (chunk.constants.count(Bx)) {
                registers[A] = Bx; // Stocke l'index de la constante dans le registre
                std::cout << "GETGLOBAL : R(" << A << ") <- " << chunk.constants[Bx] << std::endl;
            } else {
                std::cerr << "[ERREUR] GETGLOBAL : Constante introuvable pour Bx = " << Bx << std::endl;
            }
            break;
        }
        /**case 5: { // GETGLOBAL
            int A = instr.A;
            int Bx = instr.Bx;

            if (chunk.constants.count(Bx)) {
                std::string globalName = chunk.constants[Bx];

                // Vérifier si `globals` contient la variable globale
                if (globals.count(globalName)) {
                    if (std::holds_alternative<LuaChunk>(globals[globalName])) {
                        registers[A] = reinterpret_cast<intptr_t>(&std::get<LuaChunk>(globals[globalName]));
                        std::cout << "[GETGLOBAL] Fonction utilisateur '" << globalName << "' chargée dans R(" << A << ")\n";
                    } else {
                        std::cerr << "[ERREUR] GETGLOBAL : " << globalName << " n'est pas une fonction utilisateur\n";
                    }
                } else {
                    std::cerr << "[ERREUR] GLOBAL non défini : " << globalName << std::endl;
                }
            }
            break;
        }**/

        case 7: { // SETGLOBAL
            int A = instr.A;
            int Bx = instr.Bx;

            if (chunk.constants.count(Bx)) {
                std::string globalName = chunk.constants[Bx];

                // Vérifie si la valeur dans R(A) est bien une fonction utilisateur (LuaChunk)
                if (registers[A]) {
                    globals[globalName] = *reinterpret_cast<LuaChunk*>(registers[A]);
                    std::cout << "[SETGLOBAL] Fonction '" << globalName << "' stockée dans globals\n";
                } else {
                    std::cerr << "[ERREUR] SETGLOBAL : Impossible de stocker une valeur invalide\n";
                }
            } else {
                std::cerr << "[ERREUR] SETGLOBAL : Impossible de récupérer le nom\n";
            }
            break;
        }




        case 12: // ADD
            registers[instr.A] = registers[instr.B] + registers[instr.C];
            std::cout << "ADD : " << static_cast<int>(instr.A) << " <-" << registers[instr.B] << " + " << registers[instr.C] << std::endl;
            std::cout << "RES : " << registers[instr.A] << std::endl;
            break;

        case 13: // SUB
            registers[instr.A] = registers[instr.B] - registers[instr.C];
            break;

        case 14: // MUL
            registers[instr.A] = registers[instr.B] * registers[instr.C];
            break;

        case 15: // DIV
            if (registers[instr.C] != 0)
                registers[instr.A] = registers[instr.B] / registers[instr.C];
            else
                std::cerr << "Erreur : division par zéro\n";
            break;

        case 22: // JMP
            pc += instr.sBx;
            return;  // Éviter le pc++ dans run()

        case 23: // EQ
            if (registers[instr.B] == registers[instr.C] == instr.A) {
                pc++;
            }
            break;

        case 28: { // CALL
            int A = instr.A;
            int B = instr.B - 1; // Nombre d'arguments

            // Vérifier si c'est une fonction interne (C++)
            if (chunk.constants.count(registers[A])) {
                std::string functionName = chunk.constants[registers[A]];
                if (globals.count(functionName)) {
                    if (std::holds_alternative<std::function<void(LuaRunner&, int, int)>>(globals[functionName])) {
                        auto func = std::get<std::function<void(LuaRunner&, int, int)>>(globals[functionName]);
                        func(*this, A, B); // Appel de la fonction
                        return;
                    }
                }
            }

            // Vérifier si c'est une fonction utilisateur (LuaChunk)
            if (registers[A]) {
                LuaChunk* functionPtr = reinterpret_cast<LuaChunk*>(registers[A]);
                if (functionPtr) {
                    std::cout << "[CALL] Exécution de la fonction utilisateur " << functionPtr->name << "\n";
                    executeFunction(*functionPtr, A, B);
                } else {
                    std::cerr << "[ERREUR] Aucune fonction définie en R(" << A << ")\n";
                }
            }
            break;
        }


        case 30: {// RETURN
            int A = instr.A;
            int B = instr.B - 1; // Nombre de valeurs à retourner

            std::cout << "[OP_RETURN] Returning " << B << " value(s) from R(" << A << ")\n";

            // Si B == 0, la fonction retourne 0 valeurs (cas RETURN 0 1)
            if (B == 0) {break;}

            // Gestion du retour de valeurs
            std::vector<int> returnValues;
            for (int i = 0; i < B; i++) {
                returnValues.push_back(registers[A + i]);
            }

            // Simule le retour (dans une VM complète, il faudrait gérer la pile d'appels)
            for (int val : returnValues) {
                std::cout << "Returned value: " << val << "\n";
            }

            break;
        }

        default:
            std::cerr << "Opcode inconnu : " << (int)instr.opcode << "\n";
            break;
    }
}


void LuaRunner::run() {
    std::cout << "**********************************\n";
    std::cout << "=== Début de l'exécution ===\n";

    while (pc < chunk.instructions.size()) {
        LuaInstructions instr = chunk.instructions[pc];
        LuaRunner::executeInstruction(instr);
        pc++;
    }

    std::cout << "=== Fin de l'exécution ===\n";
}

