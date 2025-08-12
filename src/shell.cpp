#include "shell.hpp"
#include "cluster.hpp"
#include "fat_manager.hpp"
#include "directory.hpp"
#include "file.hpp"
#include "fat.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>

static std::vector<std::pair<std::string, uint16_t>> dir_stack = {{"", 9}}; // "" representa a raiz

// Remove a referência global e substitui por funções de acesso
uint16_t get_current_cluster() {
    return dir_stack.back().second;
}

void update_current_path(const std::string& name, uint16_t cluster) {
    dir_stack.emplace_back(name, cluster);
}

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
            auto entries = list_directory(get_current_cluster());
            if (entries.empty()) {
                std::cout << "";
            } else {
                for (const auto& e : entries) {
                    // Verificação final de segurança
                    if (strcmp(e.filename, ".") != 0 && strcmp(e.filename, "..") != 0) {
                        std::cout << (e.attributes == 0x01 ? "[DIR] " : "[FILE] ");
                        std::cout << e.filename << " (" << e.size << " bytes)\n";
                    }
                }
            }
        }

        else if (cmd == "mkdir") {
            std::string name;
            iss >> name;
            if (name.empty()) {
                std::cerr << "Uso: mkdir <nome>\n";
                continue;
            }
            if (!create_directory(name, get_current_cluster())) {
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
            if (!create_file(name, get_current_cluster())) {
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

            if (!write_file(name, data, get_current_cluster())) {
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

            auto data = read_file(name, get_current_cluster());
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

            if (!delete_file(name, get_current_cluster())) {
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
                }
            } else {
                auto entry = find_entry(name, get_current_cluster());
                if (entry && entry->attributes == 0x01) {
                    if (strcmp(entry->filename, ".") != 0 && strcmp(entry->filename, "..") != 0) {
                        if (entry->first_block >= 10 && entry->first_block < NUM_CLUSTERS) {
                            update_current_path(name, entry->first_block);
                        } else {
                            std::cerr << "Erro: cluster do diretório inválido.\n";
                        }
                    }
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