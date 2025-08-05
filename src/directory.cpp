#include "directory.hpp"
#include "cluster.hpp"
#include "fat_manager.hpp"
#include <cstring>
#include <iostream>

bool create_directory(const std::string& dirname, uint16_t parent_cluster) {
    DirEntry new_dir = {};
    std::strncpy(new_dir.filename, dirname.c_str(), sizeof(new_dir.filename) - 1);
    new_dir.filename[sizeof(new_dir.filename) - 1] = '\0';
    new_dir.attributes = 0x01; // diretório
    new_dir.size = 0;

    int new_cluster = find_free_fat_block();
    if (new_cluster == -1) {
        std::cerr << "Erro: Sem espaço disponível para criar diretório.\n";
        return false;
    }

    new_dir.first_block = static_cast<uint16_t>(new_cluster);

    if (!add_entry_to_directory(new_dir, parent_cluster)) {
        std::cerr << "Erro: Falha ao adicionar entrada no diretório pai.\n";
        return false;
    }

    DataCluster cluster = {};
    write_cluster(new_cluster, cluster);
    return true;
}

std::vector<DirEntry> list_directory(uint16_t cluster_id) {
    std::vector<DirEntry> entries;
    DataCluster cluster;
    load_cluster(cluster_id, cluster);

    for (const DirEntry& entry : cluster.dir) {
        if (entry.filename[0] != '\0') {
            entries.push_back(entry);
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
