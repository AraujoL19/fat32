#include "file.hpp"
#include "cluster.hpp"
#include "fat_manager.hpp"
#include "directory.hpp"
#include <cstring>
#include <iostream>

bool create_file(const std::string& filename, uint16_t parent_cluster) {
    if (find_entry(filename, parent_cluster).has_value()) {
        std::cerr << "Já existe um arquivo ou diretório com esse nome.\n";
        return false;
    }

    int cluster_index = find_free_fat_block();
    if (cluster_index == -1) {
        std::cerr << "Sem espaço na FAT.\n";
        return false;
    }

    DirEntry file_entry = {};
    std::strncpy(file_entry.filename, filename.c_str(), sizeof(file_entry.filename) - 1);
    file_entry.attributes = 0x02; // arquivo
    file_entry.first_block = static_cast<uint16_t>(cluster_index);
    file_entry.size = 0;

    DataCluster empty = {};
    write_cluster(cluster_index, empty);

    return add_entry_to_directory(file_entry, parent_cluster);
}

bool write_file(const std::string& filename, const std::vector<char>& data, uint16_t parent_cluster) {
    auto opt_entry = find_entry(filename, parent_cluster);
    if (!opt_entry.has_value()) {
        std::cerr << "Arquivo não encontrado.\n";
        return false;
    }

    DirEntry entry = opt_entry.value();
    if (entry.attributes != 0x02) {
        std::cerr << "Erro: não é possível escrever em um diretório.\n";
        return false;
    }

    int current = entry.first_block;

    // Limpa clusters antigos
    while (fat[current] != 0xFFFF) {
        int next = fat[current];
        fat[current] = 0x0000;
        current = next;
    }
    fat[current] = 0x0000; // limpa o último

    // Grava novos dados
    size_t bytes_written = 0;
    int first_cluster = -1;
    int previous_cluster = -1;

    while (bytes_written < data.size()) {
        int new_cluster = find_free_fat_block();
        if (new_cluster == -1) {
            std::cerr << "Erro: espaço insuficiente.\n";
            return false;
        }

        if (first_cluster == -1)
            first_cluster = new_cluster;

        if (previous_cluster != -1)
            fat[previous_cluster] = new_cluster;

        DataCluster cluster = {};
        size_t to_write = std::min(static_cast<size_t>(CLUSTER_SIZE), data.size() - bytes_written);
        std::memcpy(cluster.data, data.data() + bytes_written, to_write);
        write_cluster(new_cluster, cluster);

        bytes_written += to_write;
        previous_cluster = new_cluster;
    }

    fat[previous_cluster] = 0xFFFF;

    entry.first_block = static_cast<uint16_t>(first_cluster);
    entry.size = data.size();

    // Atualiza entrada
    remove_entry_from_directory(filename, parent_cluster);
    add_entry_to_directory(entry, parent_cluster);
    save_fat();

    return true;
}

std::optional<std::vector<char>> read_file(const std::string& filename, uint16_t parent_cluster) {
    auto opt_entry = find_entry(filename, parent_cluster);
    if (!opt_entry.has_value()) return std::nullopt;

    DirEntry entry = opt_entry.value();
    if (entry.attributes != 0x02) {
        std::cerr << "Erro: não é um arquivo.\n";
        return std::nullopt;
    }

    std::vector<char> buffer;
    int current = entry.first_block;
    size_t total = 0;

    while (current != 0xFFFF && total < entry.size) {
        DataCluster cluster;
        load_cluster(current, cluster);

        size_t to_read = std::min(static_cast<size_t>(CLUSTER_SIZE), entry.size - total);
        buffer.insert(buffer.end(), cluster.data, cluster.data + to_read);
        total += to_read;

        current = fat[current];
    }

    return buffer;
}

bool delete_file(const std::string& filename, uint16_t parent_cluster) {
    auto opt_entry = find_entry(filename, parent_cluster);
    if (!opt_entry.has_value()) return false;

    DirEntry entry = opt_entry.value();
    if (entry.attributes != 0x02) {
        std::cerr << "Erro: não é um arquivo.\n";
        return false;
    }

    int current = entry.first_block;
    while (current != 0xFFFF) {
        int next = fat[current];
        fat[current] = 0x0000;
        current = next;
    }

    remove_entry_from_directory(filename, parent_cluster);
    save_fat();
    return true;
}