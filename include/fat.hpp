//  definir  uma  lista  encadeada  simples  das  "extensões"  (clusters)  de  um  arquivo.
// A  FAT  mapeia  a  região  de  dados  do  volume  por  número  de  cluster.  O  primeiro  cluster  de  dados  é  o  cluster  2.
#ifndef FAT_HPP
#define FAT_HPP
#include <cstdint>
#include "bpb.hpp"
#include "cluster.hpp"
#include <array>
#include <cstdint>
#include "fat_utils.hpp"
namespace FAT {
    uint32_t FATOffset(uint32_t cluster, const BPB& bpb) {
        uint32_t FATSz;
        if (bpb.BS_FATSz16 != 0) {
            FATSz = bpb.BS_FATSz16;
        } else {
            FATSz = bpb.BS_FATSz32;
        }

        if (FATType(bpb) == 16) {
            return (cluster * 2);
        } else if (FATType(bpb) == 32){
            return (cluster * 4);
        }
    }

    uint32_t ThisFATSecNum(uint32_t cluster, const BPB& bpb) {
        return bpb.BS_RsvdSecCnt + (FATOffset(cluster, bpb) / bpb.BS_BytsPerSec);
    }

    uint32_t ThisFATEntOffset(uint32_t cluster, const BPB& bpb) {
        return FATOffset(cluster, bpb) % bpb.BS_BytsPerSec;
    }

    uint32_t FAT32ClusEntryVal(uint32_t cluster, const BPB& bpb, const uint8_t* SecBuff) {
        uint32_t FAT32ClusEntryVal = (*((const uint32_t *) &SecBuff[ThisFATEntOffset(cluster, bpb)])) & 0x0FFFFFFF;
        FAT32ClusEntryVal = FAT32ClusEntryVal & 0x0FFFFFFF; 
        *((DWORD *) &SecBuff[ThisFATEntOffset(cluster, bpb)]) = 
        (*((DWORD *) &SecBuff[ThisFATEntOffset(cluster, bpb)])) & 0xF0000000; 
        *((DWORD *) &SecBuff[ThisFATEntOffset(cluster, bpb)]) =  
        (*((DWORD *) &SecBuff[ThisFATEntOffset(cluster, bpb)])) | FAT32ClusEntryVal;
    }

    uint32_t CalculateBPB_FATSz32(const BPB& bpb, uint32_t DskSize) {
        uint32_t TmpVal1 = DskSize - (bpb.BS_RsvdSecCnt + 0);
        uint32_t TmpVal2 = (256 * bpb.BS_SecPerClus) + bpb.BS_NumFATs;
        if (FATType(bpb) == 32) {
            TmpVal2 = TmpVal2 / 2;
        }
        uint32_t FATSz = (TmpVal1 + (TmpVal2 - 1)) / TmpVal2;
        return FATSz;
    }
}

#endif // FAT_HPP