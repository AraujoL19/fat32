#include <cstdint>
#include "bpb.hpp"
#include <iostream>
#include <iomanip>
#include <string>

bool readBPB(std::ifstream &img, BPB &bpb)
{
    img.seekg(0); // Move o "ponteiro de leitura" para a posição 0 (início do arquivo).
    img.read(reinterpret_cast<char*>(&bpb), sizeof(BPB)); //Lê sizeof(BPB) bytes do arquivo e armazena na variável bpb.
    return img.good(); // Retorna true se a leitura foi bem-sucedida, caso contrário, retorna false.
}

void printBPB(const BPB &bpb)
{
    std::cout << "Jump Instruction: " << std::hex << std::setw(2) << std::setfill('0') // muda a saída de números inteiros para base hexadecimal e preenche com zeros à esquerda
              << static_cast<int>(bpb.BS_jmpBoot[0]) << " "
              << static_cast<int>(bpb.BS_jmpBoot[1]) << " "
              << static_cast<int>(bpb.BS_jmpBoot[2]) << std::dec << std::endl;
    std::cout << "OEM Name: " << std::string(bpb.BS_OEMName) << std::endl;
    std::cout << "Bytes Per Sector: " << bpb.BS_BytsPerSec << std::endl;
    std::cout << "Sectors Per Cluster: " << static_cast<int>(bpb.BS_SecPerClus) << std::endl;
    std::cout << "Reserved Sectors: " << bpb.BS_RsvdSecCnt << std::endl;
    std::cout << "Number of FATs: " << static_cast<int>(bpb.BS_NumFATs) << std::endl;
    std::cout << "Root Entries: " << bpb.BS_RootEntCnt << std::endl;
    std::cout << "Total Sectors (16-bit): " << bpb.BS_TotSec16 << std::endl;
    std::cout << "Media Type: 0x" << std::hex
              << static_cast<int>(bpb.BS_Media) << std::dec << std::endl;
    std::cout << "FAT Size (16-bit): " << bpb.BS_FATSz16 << std::endl;
    std::cout << "Sectors Per Track: " << bpb.BS_SecPerTrk << std::endl;
    std::cout << "Number of Heads: " << bpb.BS_NumHeads << std::endl;
    std::cout << "Hidden Sectors: " << bpb.BS_HiddSec << std::endl;
    std::cout << "Total Sectors (32-bit): "
              << (bpb.BS_TotSec16 == 0 ? bpb.BS_TotSec32 : bpb.BS_TotSec16)
              << std::endl;
    std::cout << "FAT Size (32-bit): " << bpb.BS_FATSz32 << std::endl;
    std::cout << "Root Cluster: " << bpb.BS_RootClus << std::endl;
    std::cout << "FS Info Sector: " << bpb.BS_FSInfo << std::endl;
    std::cout << "Backup Boot Sector: " << bpb.BS_BkBootSec << std::endl;
    std::cout << "Volume ID: " << bpb.BS_VolID << std::endl;
    std::cout << "Volume Label: " << std::string(bpb.BS_VolLab, 11) << std::endl;
    std::cout << "Filesystem Type: " << std::string(bpb.BS_FileSysType, 8) << std::endl;
}