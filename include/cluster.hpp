#include <cstdint>
#include "bpb.hpp"
#ifndef CLUSTER_HPP
#define CLUSTER_HPP

//RootDirSectors = ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec – 1)) / BPB_BytsPerSec;
//Em  um  volume  FAT32  o  valor  BPB_RootEntCnt  é  sempre  0,  portanto,  em  um  volume  FAT32,  RootDirSectors  é  sempre  0.
constexpr uint32_t RootDirSectors = 0;

uint32_t FirstDataSector(const BPB& bpb) {
    uint32_t FATSz;
    if (bpb.BS_FATSz16 != 0) {
        FATSz = bpb.BS_FATSz16;
    } else {
        FATSz = bpb.BS_FATSz32;
    }
    return bpb.BS_RsvdSecCnt + (bpb.BS_NumFATs * FATSz) + RootDirSectors;
}

uint32_t FirstSectorOfCluster(uint32_t cluster, const BPB& bpb) {
    return ((cluster - 2) * bpb.BS_SecPerClus) + FirstDataSector(bpb);
}

uint32_t DataSec(const BPB& bpb) {
    uint32_t FATSz;
    if (bpb.BS_FATSz16 != 0) {
        FATSz = bpb.BS_FATSz16;
    } else {
        FATSz = bpb.BS_FATSz32;
    }
    uint32_t TotSec;
    if (bpb.BS_TotSec16 != 0) {
        TotSec = bpb.BS_TotSec16;
    } else {
        TotSec = bpb.BS_TotSec32;
    }
    return TotSec - (bpb.BS_RsvdSecCnt + (bpb.BS_NumFATs * FATSz) + RootDirSectors);
}

uint32_t CountOfClusters(const BPB& bpb) {
    return DataSec(bpb) / bpb.BS_SecPerClus;
}

#endif // CLUSTER_HPP