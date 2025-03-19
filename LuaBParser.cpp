//
// Created by Shaimani on 18/03/2025.
//

//#include "LuaBParser.h"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <unordered_map>

struct LuaChunk {
    std::string name;
    int first_line;
    int last_line;
    uint8_t upvalues;
    uint8_t arguments;
    uint8_t vararg;
    uint8_t stack_size;
    std::vector<uint32_t> instructions;
    std::unordered_map<int, std::string> constants;
    std::vector<LuaChunk> prototypes;
};

class LuaBParser {
private :
    unsigned char version;
    unsigned char format;
    unsigned char endianness;
    unsigned char int_size;
    unsigned char size_t_size;
    unsigned char instruction_size;
    unsigned char lua_number_size;
    unsigned char integral_flag;

    LuaChunk chunk;
    std::vector<uint8_t> bytecode;
    size_t index = 0;

    uint8_t get_byte() {
        return bytecode[index++];
    }

    uint32_t get_int() {
        uint32_t value = 0;
        if (big_endian) {
            for (int i = 0; i < int_size; i++) {
                value = (value << 8) | get_byte();  // Lire en Big-endian
            }
        } else {
            for (int i = 0; i < int_size; i++) {
                value |= get_byte() << (i * 8);  // Lire en Little-endian
            }
        }
        return value;
    }
    size_t get_size_t() {
        size_t value = 0;
        if (big_endian) {
            for (int i = 0; i < size_t_size; i++) {
                value = (value << 8) | get_byte();  // Lire en Big-endian
            }
        } else {
            for (int i = 0; i < size_t_size; i++) {
                value |= get_byte() << (i * 8);  // Lire en Little-endian
            }
        }
        return value;
    }


    double get_lua_number() {
        double value;
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);

        if (big_endian) {
            for (int i = 0; i < lua_number_size; i++) {
                ptr[i] = get_byte();  // Lire en Big-endian
            }
        } else {
            for (int i = 0; i < lua_number_size; i++) {
                ptr[7 - i] = get_byte();  // Lire en Little-endian
            }
        }
        return value;
    }

    std::string get_string() {
        size_t size = get_size_t();  // Lire la taille de la chaîne

        if (size == 0) return "";  // Si size == 0, pas de chaîne

        std::string str(bytecode.begin() + index, bytecode.begin() + index + size - 1);
        index += size;  // Avancer de toute la taille, incluant le NUL

        return str;  // Retourne la chaîne sans le NUL
    }

public :
    bool big_endian=false;
    void load(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if(!file) {
            std::cerr << "Erreur : Impossible d'ouvrir le fichier " << filename << std::endl;
            return;
        }

        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0, std::ios::beg);

        bytecode.resize(size);
        file.read(reinterpret_cast<char*>(bytecode.data()), size);
        file.close();
    }

    /**
     * Parse l'en-tête du bytecode Lua 5.1
     * @return true si l'en-tête a été parsée avec succès, false sinon
     */

    bool parse_lua_header() {

        // Extraction des champs de l'en-tête
        std::vector<uint8_t> signature(bytecode.begin(), bytecode.begin() + 4);
        version = bytecode[4];
        format = bytecode[5];
        endianness = bytecode[6];
        int_size = bytecode[7];
        size_t_size = bytecode[8];
        instruction_size = bytecode[9];
        lua_number_size = bytecode[10];
        integral_flag = bytecode[11];
        index = 12;

        // Affichage des informations
        std::cout << "===== En-tête du bytecode Lua 5.1 =====\n";
        std::cout << "Signature : ";
        for (unsigned char c : signature) {
            std::cout << std::hex << std::uppercase << (int)c << " ";
        }
        std::cout << " (Lua ?) \n";
        std::cout << "Version : 5." << (version & 0x0F) << " (Hex: 0x" << std::hex << static_cast<int>(version) << ")\n";
        std::cout << "Format : " << (format == 0 ? "Officiel" : "Non officiel") << "\n";
        std::cout << "Endianness : " << (endianness == 1 ? "Little-endian" : "Big-endian") << "\n";
        if(endianness == 0){big_endian = true;}
        std::cout << "Taille de int : " << std::dec << static_cast<int>(int_size) << " octets\n";
        std::cout << "Taille de size_t : " << static_cast<int>(size_t_size) << " octets\n";
        std::cout << "Taille des instructions : " << static_cast<int>(instruction_size) << " octets\n";
        std::cout << "Taille de lua_Number : " << static_cast<int>(lua_number_size) << " octets\n";
        std::cout << "Type de lua_Number : " << (integral_flag ? "Entier" : "Flottant") << "\n";
        std::cout << "=======================================\n";

        return true;
    }

    LuaChunk decode_chunk(){
        chunk.name = get_string();
        chunk.first_line = get_int();
        chunk.last_line = get_int();
        chunk.upvalues = get_byte();
        chunk.arguments = get_byte();
        chunk.vararg = get_byte();
        chunk.stack_size = get_byte();

        std::cout << "** DECODING CHUNK **\n";
        std::cout << "Function Name: " << chunk.name << "\n";
        std::cout << "First Line: " << chunk.first_line << "\n";
        std::cout << "Last Line: " << chunk.last_line << "\n";
        std::cout << "Upvalues: " << static_cast<int>(chunk.upvalues) << "\n";
        std::cout << "Arguments: " << static_cast<int>(chunk.arguments) << "\n";
        std::cout << "Vararg: " << static_cast<int>(chunk.vararg) << "\n";
        std::cout << "Stack Size: " << static_cast<int>(chunk.stack_size) << "\n";
        decode_instructions(chunk);
        decode_constants(chunk);

        return chunk;
    }

    void decode_instructions(LuaChunk& chunk) {
        uint32_t num_instructions = get_int();
        std::cout << "** DECODING INSTRUCTIONS (" << num_instructions << ") **\n";

        for (uint32_t i = 0; i < num_instructions; i++) {
            uint32_t instr = get_int();
            chunk.instructions.push_back(instr);

            uint8_t opcode = instr & 0x3F;
            uint8_t A = (instr >> 6) & 0xFF;
            uint16_t B = (instr >> 23) & 0x1FF;
            uint16_t C = (instr >> 14) & 0x1FF;
            uint32_t Bx = (instr >> 14) & 0x3FFFF;
            int32_t sBx = Bx - 131071;

            std::cout << "Instruction #" << i << " - Opcode: " << static_cast<int>(opcode)
                      << ", A: " << static_cast<int>(A)
                      << ", B: " << static_cast<int>(B)
                      << ", C: " << static_cast<int>(C)
                      << ", Bx: " << Bx
                      << ", sBx: " << sBx << std::endl;
        }
    }
    void decode_constants(LuaChunk& chunk) {
        uint32_t num_constants = get_int();
        std::cout << "** DECODING CONSTANTS (" << num_constants << ") **\n";

        for (uint32_t i = 0; i < num_constants; i++) {
            uint8_t type = get_byte();
            std::string value;

            switch (type) {
                case 0:
                    value = "nil";
                    break;
                case 1:
                    value = (get_byte() ? "true" : "false");
                    break;
                case 3:
                    value = std::to_string(get_size_t());
                    break;
                case 4:
                    value = get_string();
                    break;
                default:
                    value = "UNKNOWN";
                    break;
            }

            chunk.constants[i] = value;
            std::cout << "Constant #" << i << ": " << value << std::endl;
        }
    }
    void decode_prototypes(LuaChunk& chunk) {
        uint32_t num_protos = get_int();
        std::cout << "** DECODING PROTOTYPES (" << num_protos << ") **\n";

        for (uint32_t i = 0; i < num_protos; i++) {
            chunk.prototypes.push_back(decode_chunk());
        }
    }

};

int main() {
    LuaBParser parser;
    parser.load("luac.out");
    parser.parse_lua_header();
    LuaChunk chunk = parser.decode_chunk();
    return 0;
}


