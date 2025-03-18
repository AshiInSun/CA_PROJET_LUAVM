//
// Created by Shaimani on 18/03/2025.
//

//#include "LuaBParser.h"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>

class LuaBParser {
private :
    std::vector<uint8_t> bytecode;
    size_t index = 0;

    uint8_t get_byte() {
        return bytecode[index++];
    }

    uint32_t get_int() {
        uint32_t value = 0;
        if (big_endian) {
            for (int i = 0; i < 4; i++) {
                value = (value << 8) | get_byte();  // Lire en Big-endian
            }
        } else {
            for (int i = 0; i < 4; i++) {
                value |= get_byte() << (i * 8);  // Lire en Little-endian
            }
        }
        return value;
    }


    double get_lua_number() {
        double value;
        uint8_t* ptr = reinterpret_cast<uint8_t*>(&value);

        if (big_endian) {
            for (int i = 0; i < 8; i++) {
                ptr[i] = get_byte();  // Lire en Big-endian
            }
        } else {
            for (int i = 0; i < 8; i++) {
                ptr[7 - i] = get_byte();  // Lire en Little-endian
            }
        }
        return value;
    }

    std::string get_string(size_t size) {
        std::string str(bytecode.begin() + index, bytecode.begin() + index + size);
        index += size;
        return str;
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
        unsigned char version = bytecode[4];
        unsigned char format = bytecode[5];
        unsigned char endianness = bytecode[6];
        unsigned char int_size = bytecode[7];
        unsigned char size_t_size = bytecode[8];
        unsigned char instruction_size = bytecode[9];
        unsigned char lua_number_size = bytecode[10];
        unsigned char integral_flag = bytecode[11];
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

};

int main() {
    LuaBParser parser;
    parser.load("luac.out");
    parser.parse_lua_header();
    return 0;
}


