#include "shell.hpp"
#include "fat_manager.hpp"
#include "directory.hpp"
#include "file.hpp"
#include <iostream>
#include <sstream>
#include <vector>

static std::vector<std::pair<std::string, uint16_t>> dir_stack = {{"", 9}}; // raiz

uint16_t& current_cluster = dir_stack.back().second;

std::string get_current_path() {
    std::string path = "/";
    for (size_t i = 1; i < dir_stack.size(); ++i) {
        path += dir_stack[i].first;
        if (i < dir_stack.size() - 1)
            path += "/";
    }
    return path;
}

void shell_loop() {
    std::string command;

    while (true) {
        std::cout << "FAT32:" << get_current_path() << "> ";
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
            if (name.empty()) {
                std::cerr << "Uso: mkdir <nome>\n";
                continue;
            }
            if (!create_directory(name, current_cluster)) {
                std::cerr << "Erro ao criar diretório.\n";
            }
        }

        else if (cmd == "touch") {
            std::string name;
            iss >> name;
            if (name.empty()) {
                std::cerr << "Uso: touch <nome>\n";
                continue;
            }
            if (!create_file(name, current_cluster)) {
                std::cerr << "Erro ao criar arquivo.\n";
            }
        }

        else if (cmd == "write") {
            std::string name, content;
            iss >> name;
            std::getline(iss, content);
            if (name.empty()) {
                std::cerr << "Uso: write <nome> <conteúdo>\n";
                continue;
            }
            if (!content.empty() && content[0] == ' ') content = content.substr(1);
            std::vector<char> data(content.begin(), content.end());
            if (!write_file(name, data, current_cluster)) {
                std::cerr << "Erro ao escrever no arquivo.\n";
            }
        }

        else if (cmd == "read") {
            std::string name;
            iss >> name;
            if (name.empty()) {
                std::cerr << "Uso: read <nome>\n";
                continue;
            }
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
            if (name.empty()) {
                std::cerr << "Uso: rm <nome>\n";
                continue;
            }
            if (!delete_file(name, current_cluster)) {
                std::cerr << "Erro ao remover arquivo.\n";
            }
        }

        else if (cmd == "cd") {
            std::string name;
            iss >> name;
            if (name.empty()) {
                std::cerr << "Uso: cd <nome>\n";
                continue;
            }

            if (name == "..") {
                if (dir_stack.size() > 1) {
                    dir_stack.pop_back();
                } else {
                    std::cout << "Você já está no diretório raiz/\n";
                }
            } else {
                auto entry = find_entry(name, current_cluster);
                if (entry && entry->attributes == 0x01) {
                    if (entry->first_block < NUM_CLUSTERS && fat[entry->first_block] != 0x0000) {
                        dir_stack.emplace_back(name, entry->first_block);
                    } else {
                        std::cerr << "Diretório corrompido ou cluster inválido.\n";
                    }
                } else {
                    std::cerr << "Diretório não encontrado ou não é um diretório.\n";
                }
            }
        }

        else {
            std::cerr << "Comando não reconhecido.\n";
        }
    }
}