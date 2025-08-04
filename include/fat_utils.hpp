#ifndef FAT_UTILS_HPP
#define FAT_UTILS_HPP

#include "bpb.hpp"
#include "fat.hpp"

typedef uint32_t DWORD;
struct DSKSZTOSECPERCLUS {
    uint32_t DiskSize;
    uint32_t SecPerClusVal;
};

constexpr DSKSZTOSECPERCLUS DskTableFAT32[] = {
    { 66600,  0}, /*  discos  de  até  32,5  MB,  o  valor  0  para  SecPerClusVal  gera  um  erro  */
    { 532480,  1}, /*  discos  de  até  260  MB,  cluster  de  0,5k  */
    { 16777216,  8}, /*  discos  de  até  8  GB,  cluster  de  4k  */
    { 33554432,  16}, /*  discos  de  até  16  GB,  cluster  de  8k  */
    { 67108864,  32}, /*  discos  de  até  32  GB,  cluster  de  16k  */
    { 0xFFFFFFFF,  64}/*  discos  maiores  que  32  GB,  cluster  de  32  k  */
};

uint8_t FATType(const BPB& bpb) {
    uint32_t count = CountOfClusters(bpb);
    if (count < 4085) {
        return 12; // FAT12
    } else if (count < 65525) {
        return 16; // FAT16
    } else {
        return 32; // FAT32
    }
}

// Observe  que  um  arquivo  de  comprimento  zero  —  um  arquivo  sem  dados  alocados  —  tem  um  primeiro  número  de  cluster  igual  a  0  em  sua  entrada  de  diretório.  
// Este  local  de  cluster  na  FAT  contém  uma  marca  EOC  (End  Of  Clusterchain)  ou  o  número  de  cluster  do  próximo  cluster  do  arquivo. 
bool IsEOF(uint32_t FATContent, uint8_t FATType) {
    if (FATType == 12) {
        return (FATContent >= 0x0FF8);
    } else if (FATType == 16) {
        return (FATContent >= 0xFFF8);
    } else if (FATType == 32) {
        return (FATContent >= 0x0FFFFFF8);
    }
    return false; // Invalid FAT type
}

// Há  também  uma  marcação  especial  "BAD  CLUSTER".  Qualquer  cluster  que  contenha  o  valor  "BAD  CLUSTER"  em  
// sua  entrada  FAT  é  um  cluster  que  não  deve  ser  colocado  na  lista  de  livres,  pois  é  propenso  a  erros  de  disco.  
// O  valor  "BAD CLUSTER"  é  0x0FF7  para  FAT12,  0xFFF7  para  FAT16  e  0x0FFFFFF7  para  FAT32.
bool IsBadCluster(uint32_t FATContent, uint8_t FATType) {
    if (FATType == 12) {
        return (FATContent == 0x0FF7);
    } else if (FATType == 16) {
        return (FATContent == 0xFFF7);
    } else if (FATType == 32) {
        return (FATContent == 0x0FFFFFF7);
    }
    return false; // Invalid FAT type
}

//  ClnShutBitMask  =  0x08000000 para FAT32
//  Se  o  bit  for  1,  o  volume  está  “limpo”.
//  Se  o  bit  for  0,  o  volume  está  "sujo".  Isso  indica  que  o  driver  do  sistema  de  arquivos  não  desmontou  o  volume  corretamente  na  última  vez  em  que  o  montou.
bool IsCleanShutdown(uint32_t FATContent) {
    return (FATContent & 0x08000000) == 1;
}

//  HrdErrBitMask  =  0x04000000 para FAT32
//   Se  este  bit  for  1,  nenhum  erro  de  leitura/gravação  de  disco  foi  encontrado.
//  Se  este  bit  for  0,  o  driver  do  sistema  de  arquivos  encontrou  um  erro  de  E/S  de  disco  no  volume  na  última  vez  em  que  foi  montado,  
// o  que  é  um  indicador  de  que  alguns  setores  podem  ter  apresentado  problemas  no  volume.
bool HasHardError(uint32_t FATContent) {
    return (FATContent & 0x04000000) != 1;
}

#endif // FAT_UTILS_HPP