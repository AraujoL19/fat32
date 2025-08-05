#include "fat.hpp"
#include <array>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

std::array<uint16_t, NUM_CLUSTERS> fat = {};
std::array<uint8_t, CLUSTER_SIZE> boot_block = {};

void init_fs() {
    std::ofstream file(FAT_FILENAME, std::ios::binary | std::ios::trunc);

    std::fill(boot_block.begin(), boot_block.end(), 0xbb);
    file.write(reinterpret_cast<char*>(boot_block.data()), boot_block.size());

    fat[0] = 0xfffd;
    for (int i = 1; i < 9; ++i) fat[i] = 0xfffe;
    fat[9] = 0xffff;
    for (int i = 10; i < NUM_CLUSTERS; ++i) fat[i] = 0x0000;

    file.write(reinterpret_cast<char*>(fat.data()), fat.size() * sizeof(uint16_t));

    // root dir vazio
    DirEntry root_dir[32] = {};
    file.write(reinterpret_cast<char*>(root_dir), sizeof(root_dir));

    // clusters vazios
    DataCluster empty = {};
    for (int i = 0; i < NUM_CLUSTERS - 10; ++i)
        file.write(reinterpret_cast<char*>(&empty), sizeof(DataCluster));

    file.close();
}

void load_fat() {
    std::ifstream file(FAT_FILENAME, std::ios::binary);
    file.seekg(CLUSTER_SIZE);
    file.read(reinterpret_cast<char*>(fat.data()), fat.size() * sizeof(uint16_t));
    file.close();
}

void save_fat() {
    std::fstream file(FAT_FILENAME, std::ios::binary | std::ios::in | std::ios::out);
    file.seekp(CLUSTER_SIZE);
    file.write(reinterpret_cast<char*>(fat.data()), fat.size() * sizeof(uint16_t));
    file.close();
}

int find_free_fat_block() {
    for (int i = 10; i < NUM_CLUSTERS; ++i) {
        if (fat[i] == 0x0000) {
            fat[i] = 0xffff;
            save_fat();
            return i;
        }
    }
    return -1;
}