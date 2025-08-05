#include "shell.hpp"
#include "fat_manager.hpp"
#include "directory.hpp"
#include "file.hpp"
#include <iostream>
#include <sstream>

static int current_cluster = 9; // Root

void shell_loop() {
    std::string command;

    while (true) {
        std::cout << "FAT32> ";
        std::getline(std::cin, command);
        std::istringstream iss(command);
        std::string cmd;
        iss >> cmd;

        if (cmd == "exit") break;

        else if (cmd == "ls") {
            auto entries = list_directory(current_cluster);
            for (const auto& e : entries) {
                std::cout << (e.attributes == 0x01 ? "[DIR] " : "[FILE] ");
                std::cout << e.filename << " (" << e.size << " bytes)\n";
            }
        }
        else if (cmd == "mkdir") {
            std::string name;
            iss >> name;
            if (!create_directory(name, current_cluster)) {
                std::cerr << "Erro ao criar diretório.\n";
            }
        }
        else if (cmd == "touch") {
            std::string name;
            iss >> name;
            if (!create_file(name, current_cluster)) {
                std::cerr << "Erro ao criar arquivo.\n";
            }
        }
        else if (cmd == "write") {
            std::string name, content;
            iss >> name;
            std::getline(iss, content);
            content = content.substr(1); // remove espaço inicial
            if (!write_file(name, std::vector<char>(content.begin(), content.end()), current_cluster)) {
                std::cerr << "Erro ao escrever no arquivo.\n";
            }
        }
        else if (cmd == "read") {
            std::string name;
            iss >> name;
            auto data = read_file(name, current_cluster);
            if (data) {
                std::cout << std::string(data->begin(), data->end()) << "\n";
            } else {
                std::cerr << "Erro ao ler arquivo.\n";
            }
        }
        else if (cmd == "rm") {
            std::string name;
            iss >> name;
            if (!delete_file(name, current_cluster)) {
                std::cerr << "Erro ao remover arquivo.\n";
            }
        }
        else if (cmd == "cd") {
            std::string name;
            iss >> name;
            if (name == "..") {
                std::cout << "cd .. ainda não implementado.\n";
            } else {
                auto entry = find_entry(name, current_cluster);
                if (entry && entry->attributes == 0x01) {
                    current_cluster = entry->first_block;
                } else {
                    std::cerr << "Diretório não encontrado.\n";
                }
            }
        }
        else {
            std::cerr << "Comando não reconhecido.\n";
        }
    }
}
