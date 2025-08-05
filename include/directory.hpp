#ifndef DIRECTORY_HPP
#define DIRECTORY_HPP

#include "fat.hpp"
#include "cluster.hpp"
#include <string>
#include <optional>
#include <vector>

// Cria um novo diretório vazio em um cluster livre
bool create_directory(const std::string& dirname, uint16_t parent_cluster);

// Lista todas as entradas de um diretório
std::vector<DirEntry> list_directory(int cluster_index);

// Procura uma entrada de diretório por nome
std::optional<DirEntry> find_entry(const std::string& name, int cluster_index);

// Adiciona uma nova entrada em um diretório
bool add_entry_to_directory(const DirEntry& entry, uint16_t cluster_index);

// Remove uma entrada de um diretório por nome
bool remove_entry_from_directory(const std::string& name, uint16_t cluster_index);

#endif // DIRECTORY_HPP