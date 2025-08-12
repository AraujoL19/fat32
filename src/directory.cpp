#include "directory.hpp"
#include "cluster.hpp"
#include "fat_manager.hpp"
#include "fat.hpp"
#include "file.hpp"
#include <cstring>
#include <iostream>
#include <string>

bool create_directory(const std::string& dirname, uint16_t parent_cluster) {
    if (find_entry(dirname, parent_cluster).has_value()) {
        std::cerr << "Já existe um arquivo ou diretório com esse nome.\n";
        return false;
    }

    int new_cluster = find_free_fat_block();
    if (new_cluster == -1) {
        std::cerr << "Erro: Sem espaço disponível para criar diretório.\n";
        return false;
    }

    // Cria entrada no diretório pai
    DirEntry new_dir = {};
    std::strncpy(new_dir.filename, dirname.c_str(), sizeof(new_dir.filename) - 1);
    new_dir.attributes = 0x01;
    new_dir.first_block = static_cast<uint16_t>(new_cluster);
    new_dir.size = 0;

    if (!add_entry_to_directory(new_dir, parent_cluster)) {
        std::cerr << "Erro: Falha ao adicionar entrada no diretório pai.\n";
        return false;
    }

    // Inicializa o novo cluster do diretório
    DataCluster cluster = {};
    
    // Entrada '.' (self)
    DirEntry self_entry = {};
    std::strncpy(self_entry.filename, ".", sizeof(self_entry.filename));
    self_entry.attributes = 0x01;
    self_entry.first_block = static_cast<uint16_t>(new_cluster);
    self_entry.size = 0;
    cluster.dir[0] = self_entry;

    // Entrada '..' (parent)
    DirEntry parent_entry = {};
    std::strncpy(parent_entry.filename, "..", sizeof(parent_entry.filename));
    parent_entry.attributes = 0x01;
    parent_entry.first_block = parent_cluster;
    parent_entry.size = 0;
    cluster.dir[1] = parent_entry;

    // Limpa todas as outras entradas
    for (size_t i = 2; i < (CLUSTER_SIZE / sizeof(DirEntry)); i++) {
        cluster.dir[i] = {};
    }

    write_cluster(new_cluster, cluster);
    return true;
}

std::vector<DirEntry> list_directory(uint16_t cluster_id) {
    std::vector<DirEntry> entries;
    DataCluster cluster;
    load_cluster(cluster_id, cluster);

    for (const DirEntry& entry : cluster.dir) {
        // Filtra entradas vazias, deletadas e especiais
        if (entry.filename[0] != '\0' && 
            entry.filename[0] != 0xE5 &&
            strcmp(entry.filename, ".") != 0 && 
            strcmp(entry.filename, "..") != 0) {
            
            // Verificação adicional para evitar autorreferência
            if (!(entry.attributes == 0x01 && entry.first_block == cluster_id)) {
                entries.push_back(entry);
            }
        }
    }
    return entries;
}

std::optional<DirEntry> find_entry(const std::string& name, uint16_t cluster_id) {
    DataCluster cluster;
    load_cluster(cluster_id, cluster);

    for (const DirEntry& entry : cluster.dir) {
        if (std::strncmp(entry.filename, name.c_str(), sizeof(entry.filename)) == 0) {
            return entry;
        }
    }
    return std::nullopt;
}

bool add_entry_to_directory(const DirEntry& entry, uint16_t cluster_index) {
    DataCluster cluster;
    load_cluster(cluster_index, cluster);

    for (DirEntry& dir_entry : cluster.dir) {
        if (dir_entry.filename[0] == '\0') {
            dir_entry = entry;
            write_cluster(cluster_index, cluster);
            return true;
        }
    }
    std::cerr << "Erro: Diretório cheio. Não foi possível adicionar entrada.\n";
    return false;
}

bool remove_entry_from_directory(const std::string& name, uint16_t cluster_index) {
    DataCluster cluster;
    load_cluster(cluster_index, cluster);

    for (DirEntry& entry : cluster.dir) {
        if (std::strncmp(entry.filename, name.c_str(), sizeof(entry.filename)) == 0) {
            std::memset(&entry, 0, sizeof(DirEntry));
            write_cluster(cluster_index, cluster);
            return true;
        }
    }
    return false;
}
