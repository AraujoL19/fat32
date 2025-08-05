#ifndef FILE_HPP
#define FILE_HPP

#include "fat.hpp"
#include <string>
#include <vector>
#include <optional>

// Cria um novo arquivo vazio
bool create_file(const std::string& filename, uint16_t parent_cluster);

// Escreve dados em um arquivo (sobrescreve)
bool write_file(const std::string& filename, const std::vector<char>& data, uint16_t parent_cluster);

// Lê os dados de um arquivo
std::optional<std::vector<char>> read_file(const std::string& filename, uint16_t parent_cluster);

// Remove um arquivo
bool delete_file(const std::string& filename, uint16_t parent_cluster);

#endif // FILE_HPP