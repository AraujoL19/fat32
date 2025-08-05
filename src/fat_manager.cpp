#include "fat_manager.hpp"
#include "fat.hpp"
#include <fstream>
#include <cstring>
#include <iostream>

// Variáveis globais definidas no .cpp
std::array<uint16_t, NUM_CLUSTERS> fat = {};
std::array<uint8_t, CLUSTER_SIZE> boot_block = {};

void init_fs() {
    std::ofstream file(FAT_FILENAME, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Erro ao criar o arquivo " << FAT_FILENAME << "\n";
        return;
    }

    // Preenche o boot_block com 0xbb
    std::fill(boot_block.begin(), boot_block.end(), 0xbb);
    file.write(reinterpret_cast<const char*>(boot_block.data()), boot_block.size());

    // Inicializa a FAT
    fat[0] = 0xfffd;            // bloco reservado
    for (int i = 1; i < 9; ++i)
        fat[i] = 0xfffe;        // blocos reservados
    fat[9] = 0xffff;            // root dir
    for (int i = 10; i < NUM_CLUSTERS; ++i)
        fat[i] = 0x0000;        // livres

    file.write(reinterpret_cast<const char*>(fat.data()), fat.size() * sizeof(uint16_t));

    // Root dir vazio (32 entradas)
    DirEntry root_dir[32] = {};
    file.write(reinterpret_cast<const char*>(root_dir), sizeof(root_dir));

    // Clusters de dados (vazios)
    DataCluster empty_cluster = {};
    for (int i = 0; i < NUM_CLUSTERS - 10; ++i) {
        file.write(reinterpret_cast<const char*>(&empty_cluster), sizeof(DataCluster));
    }

    file.close();
}

void load_fat() {
    std::ifstream file(FAT_FILENAME, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo " << FAT_FILENAME << " para leitura da FAT\n";
        return;
    }

    file.seekg(CLUSTER_SIZE);  // pula o boot block
    file.read(reinterpret_cast<char*>(fat.data()), fat.size() * sizeof(uint16_t));
    file.close();
}

void save_fat() {
    std::fstream file(FAT_FILENAME, std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo " << FAT_FILENAME << " para escrita da FAT\n";
        return;
    }

    file.seekp(CLUSTER_SIZE);  // pula o boot block
    file.write(reinterpret_cast<const char*>(fat.data()), fat.size() * sizeof(uint16_t));
    file.close();
}

int find_free_fat_block() {
    for (int i = 10; i < NUM_CLUSTERS; ++i) {
        if (fat[i] == 0x0000) {
            fat[i] = 0xffff;  // marca como ocupado
            save_fat();
            return i;
        }
    }
    return -1;  // nenhum bloco livre
}
