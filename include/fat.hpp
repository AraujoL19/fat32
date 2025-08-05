#ifndef FAT_HPP
#define FAT_HPP

#include <cstdint>

// Constantes do sistema de arquivos
constexpr int SECTOR_SIZE = 512;
constexpr int CLUSTER_SIZE = 2 * SECTOR_SIZE;
constexpr int NUM_CLUSTERS = 4096;
constexpr const char* FAT_FILENAME = "fat.part";

// Estrutura de entrada de diretório (diretórios e arquivos)
struct DirEntry {
    char filename[18];          // nome do arquivo/diretório
    uint8_t attributes;         // 1: dir, 2: file
    uint8_t reserved[7];        // reservado (não utilizado)
    uint16_t first_block;       // primeiro bloco na FAT
    uint32_t size;              // tamanho do arquivo
};

// Cluster pode conter dados ou entradas de diretório
union DataCluster {
    DirEntry dir[CLUSTER_SIZE / sizeof(DirEntry)];
    char data[CLUSTER_SIZE];
};

#endif // FAT_HPP