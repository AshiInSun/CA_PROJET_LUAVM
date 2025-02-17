//
// Created by shaim on 17/02/2025.
//
#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>

bool parse_lua_header(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << filename << std::endl;
        return false;
    }

    // Lire les 12 octets de l'en-tête
    std::vector<unsigned char> header(12);
    file.read(reinterpret_cast<char*>(header.data()), header.size());

    if (file.gcount() < 12) {
        std::cerr << "Erreur : Fichier trop court pour être un bytecode Lua valide." << std::endl;
        return false;
    }

    // Extraction des champs de l'en-tête
    std::vector<unsigned char> signature(header.begin(), header.begin() + 4);
    unsigned char version = header[4];
    unsigned char format = header[5];
    unsigned char endianness = header[6];
    unsigned char int_size = header[7];
    unsigned char size_t_size = header[8];
    unsigned char instruction_size = header[9];
    unsigned char lua_number_size = header[10];
    unsigned char integral_flag = header[11];

    // Affichage des informations
    std::cout << "===== En-tête du bytecode Lua 5.1 =====\n";
    std::cout << "Signature : ";
    for (unsigned char c : signature) {
        std::cout << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)c << " ";
    }
    std::cout << " (Lua ?) \n";
    std::cout << "Version : 5." << (version & 0x0F) << " (Hex: 0x" << std::hex << static_cast<int>(version) << ")\n";
    std::cout << "Format : " << (format == 0 ? "Officiel" : "Non officiel") << "\n";
    std::cout << "Endianness : " << (endianness == 1 ? "Little-endian" : "Big-endian") << "\n";
    std::cout << "Taille de int : " << std::dec << static_cast<int>(int_size) << " octets\n";
    std::cout << "Taille de size_t : " << static_cast<int>(size_t_size) << " octets\n";
    std::cout << "Taille des instructions : " << static_cast<int>(instruction_size) << " octets\n";
    std::cout << "Taille de lua_Number : " << static_cast<int>(lua_number_size) << " octets\n";
    std::cout << "Type de lua_Number : " << (integral_flag ? "Entier" : "Flottant") << "\n";
    std::cout << "=======================================\n";

    return true;
}

int main() {
    std::string filename = "luac.out"; // Remplace par le fichier bytecode à analyser
    parse_lua_header(filename);
    return 0;
}
