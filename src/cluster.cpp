#include "cluster.hpp"
#include <iostream>

void load_cluster(int cluster_index, DataCluster& cluster) {
    if (cluster_index < 0 || cluster_index >= NUM_CLUSTERS) {
        std::cerr << "Índice de cluster inválido: " << cluster_index << "\n";
        return;
    }

    std::ifstream file(FAT_FILENAME, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo " << FAT_FILENAME << " para leitura do cluster\n";
        return;
    }

    std::streampos pos = static_cast<std::streampos>(cluster_index) * CLUSTER_SIZE;
    file.seekg(pos);
    file.read(reinterpret_cast<char*>(&cluster), sizeof(DataCluster));
    file.close();
}

void write_cluster(int cluster_index, const DataCluster& cluster) {
    if (cluster_index < 0 || cluster_index >= NUM_CLUSTERS) {
        std::cerr << "Índice de cluster inválido: " << cluster_index << "\n";
        return;
    }

    std::fstream file(FAT_FILENAME, std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo " << FAT_FILENAME << " para escrita do cluster\n";
        return;
    }

    std::streampos pos = static_cast<std::streampos>(cluster_index) * CLUSTER_SIZE;
    file.seekp(pos);
    file.write(reinterpret_cast<const char*>(&cluster), sizeof(DataCluster));
    file.close();
}

void wipe_cluster(int cluster_index) {
    DataCluster empty = {};
    write_cluster(cluster_index, empty);
}