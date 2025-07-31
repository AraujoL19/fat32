#include <cstdint>
#include "bpb.hpp"
#include <iostream>
#include <iomanip>

bool readBPB(std::ifstream &img, BPB &bpb)
{
    img.seekg(0); // Move o "ponteiro de leitura" para a posição 0 (início do arquivo).
    img.read(reinterpret_cast<char*>(&bpb), sizeof(BPB)); //Lê sizeof(BPB) bytes do arquivo e armazena na variável bpb.
    return img.good(); // Retorna true se a leitura foi bem-sucedida, caso contrário, retorna false.
}

void printBPB(const BPB &bpb)
{
    std::cout << "Jump Instruction: " << std::hex << std::setw(2) << std::setfill('0') // muda a saída de números inteiros para base hexadecimal e preenche com zeros à esquerda
              << static_cast<int>(bpb.jump[0]) << " "
              << static_cast<int>(bpb.jump[1]) << " "
              << static_cast<int>(bpb.jump[2]) << std::dec << std::endl;
    std::cout << "OEM Name: " << bpb.oem_name << std::endl;
    std::cout << "Bytes Per Sector: " << bpb.bytes_per_sector << std::endl;
    std::cout << "Sectors Per Cluster: " << static_cast<int>(bpb.sectors_per_cluster) << std::endl;
    std::cout << "Reserved Sectors: " << bpb.reserved_sectors << std::endl;
    std::cout << "Number of FATs: " << static_cast<int>(bpb.num_fats) << std::endl;
    std::cout << "Root Entries: " << bpb.root_entries << std::endl;
    std::cout << "Total Sectors (16-bit): " << bpb.total_sectors_short << std::endl;
    std::cout << "Media Type: 0x" << std::hex
              << static_cast<int>(bpb.media_type) << std::dec << std::endl;
    std::cout << "FAT Size (16-bit): " << bpb.fat_size_short << std::endl;
    std::cout << "Sectors Per Track: " << bpb.sectors_per_track << std::endl;
    std::cout << "Number of Heads: " << bpb.heads << std::endl;
    std::cout << "Hidden Sectors: " << bpb.hidden_sectors << std::endl;
    std::cout << "Total Sectors (32-bit): " 
              << (bpb.total_sectors_short == 0 ? bpb.total_sectors_long : bpb.total_sectors_short) 
              << std::endl;
}