#ifndef FAT_MANAGER_HPP
#define FAT_MANAGER_HPP

#include <array>
#include "fat.hpp"

void init_fs();               // inicializa o sistema de arquivos
void load_fat();              // carrega a FAT do disco
void save_fat();              // salva a FAT no disco
int find_free_fat_block();    // encontra um bloco livre na FAT

extern std::array<uint16_t, NUM_CLUSTERS> fat;
extern std::array<uint8_t, CLUSTER_SIZE> boot_block;

#endif // FAT_MANAGER_HPP
