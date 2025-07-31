#ifndef BPB_HPP
#define BPB_HPP

#include <cstdint>
#include <fstream>

#pragma pack(push, 1)
struct BPB {
    //0x00 - byte 0
    //BS_jmpBoot: Instrução  de  salto  para  código  de  inicialização  de  bootstrap  do  sistema  operacional.
    //Essa instrução pula os 3 bytes iniciais e continua a execução no restante do setor.
    uint8_t jump[3];
    
    //0x03 - byte 3
    //BS_OEMName: Nome do sistema que formatou o volume. Apenas decorativo; sistemas modernos ignoram.
    //Recomenda-se usar "MSWIN4.1" por compatibilidade com drivers antigos.
    char oem_name[8];
    
    //0x0B - byte 11
    //BS_BytsPerSec: Tamanho de cada setor em bytes.
    uint16_t bytes_per_sector;
    
    //0x0D - byte 13
    //BS_SecPerClus: Número de setores por cluster.
    //Deve ser potência de 2: 1, 2, 4, ..., 128
    //Regra: BytesPorSetor * SetoresPorCluster ≤ 32KB
    uint8_t sectors_per_cluster;
    
    //0x0E - byte 14
    //BS_RsvdSecCnt: Número de setores reservados antes da FAT.
    //Em FAT32, normalmente é 32.
    uint16_t reserved_sectors;
    
    //0x10 - byte 16
    //BS_NumFATs: Número de tabelas FAT. Padrão é 2 para redundância (backup).
    uint8_t num_fats;
    
    //0x11 - byte 17
    //BS_RootEntCnt: Número de entradas na raiz (FAT12/16). Em FAT32 deve ser 0, pois o diretório raiz é um cluster comum.
    uint16_t root_entries;
    
    //0x13 - byte 19
    //BS_TotSec16: Número total de setores no volume (versão de 16 bits).
    //Em FAT32, este campo deve ser 0 e o valor real está em BPB_TotSec32.
    uint16_t total_sectors_short;
    
    //0x15 - byte 21
    //BS_Media: Tipo de mídia. 0xF8: mídias fixas. 0xF0: mídias removíveis.
    uint8_t media_type;
    
    //0x16 - byte 22
    //BS_FATSz16: Tamanho da FAT (curto). 
    //Em  volumes  FAT32,  este  campo  deve  ser  0,  e  BPB_FATSz32  contém  a  contagem  do  tamanho  da  FAT. 
    uint16_t fat_size_short; 
    
    //0x18 - byte 24
    //BS_SecPerTrk: Número de setores por trilha  para  a  interrupção  0x13.  
    //Este  campo  é  relevante  apenas  para  mídias  que  possuem  uma  geometria  (o  volume  é  dividido  em  trilhas  por  várias  cabeças  e  cilindros)  e  são  visíveis  na  interrupção  0x13.
    uint16_t sectors_per_track;
    
    //0x1A - byte 26
    //BS_NumHeads: Número  de  cabeças  para  a  interrupção  0x13.
    uint16_t heads;
    
    //0x1C - byte 28
    //BS_HiddSec: Contagem  de  setores  ocultos  que  precedem  a  partição  que  contém  este  volume  FAT.  
    // Este  campo  geralmente  é  relevante  apenas  para  mídias  visíveis  na  interrupção  0x13.  
    // Este  campo  deve  ser  sempre  zero  em  mídias  não  particionadas.  
    // O  valor  apropriado  depende  do  sistema  operacional.
    uint32_t hidden_sectors;
    
    //0x20 - byte 32
    //BS_TotSec32: Número total de setores (longo).  Nova  contagem  total  de  32  bits  de  setores  no  volume. 
    //Esta  contagem  inclui  a  contagem  de  todos  os  setores  em  todas  as  quatro  regiões  do  volume.
    //Para  volumes  FAT32,  este  campo  deve  ser  diferente  de  zero.
    uint32_t total_sectors_long;

    //Apenas para FAT32:
    
    //0x24 - byte 36
    // BS_FATSz32: Tamanho da FAT (longo).
    // Este  campo  representa  a  contagem  de  32  bits  de  setores  FAT32  ocupados  por  UMA  FAT.  BPB_FATSz16  deve  ser  0.
    uint32_t fat_size_long;
    
    //0x28 - byte 40
    // BS_ExtFlags: Flags estendidas.
    // Bits  0-3  —  Número  de  FATs  ativos  baseado  em  zero.  Válido  somente  se  o  espelhamento  estiver  desabilitado.
    // Bits  4-6  —  Reservados.
    // Bit  7  --  0  significa  que  o  FAT  é  espelhado  em  tempo  de  execução  em  todos  os  FATs.--  1  significa  que  apenas  um  FAT  está  ativo;  é  o  referenciado nos  bits  0-3.
    // Bits  8-15  —  Reservados
    uint16_t ext_flags;
    
    //0x2A - byte 42
    // BS_FSVer: Versão do sistema de arquivos.
    uint16_t fs_version;
    
    //0x2C - byte 44
    // BS_RootClus: Número do cluster raiz, o  primeiro  cluster  do  diretório  raiz,  geralmente  2,  mas  não  é  obrigatório.
    uint32_t root_cluster;

    //0x30 - byte 48
    // BS_FSInfo: Número do setor FSInfo na área  reservada  do  volume  FAT32.  Normalmente  1.
    // Haverá  uma  cópia  da  estrutura  FSINFO  no  BackupBoot,  mas  somente  a  cópia  apontada  por  este  campo  será  mantida  atualizada
    //(ou  seja,  tanto  o  registro  de  inicialização  primário  quanto  o  de  backup  apontarão  para  o  mesmo  setor  FSINFO)
    uint16_t fs_info_sector;

    //0x32 - byte 50
    // BS_BkBootSec: Número do setor de backup do boot.
    // Se  diferente  de  zero,  indica  o  número  do  setor  na  área  reservada  do  volume  de  uma  cópia  do  registro  de  inicialização.
    // Geralmente  6.  Nenhum  valor  diferente  de  6  é  recomendado.
    uint16_t backup_boot_sector;
    
    //0x34 - byte 52
    // BS_Reserved: Reservado para uso futuro.
    // O  código  que  formata  volumes  FAT32  deve  sempre  definir  todos  os  bytes  deste  campo  como  0.
    uint8_t reserved[12];

    //0x40 - byte 64
    // BS_DrvNum: Número do drive.
    uint8_t drive_number;
    
    //0x41 - byte 65
    // BS_Reserved1: Reservado.
    uint8_t reserved1;
    
    //0x42 - byte 66
    // BS_BootSig: Sinalizador de boot.
    uint8_t boot_signature;
    
    //0x43 - byte 67
    // BS_VolID: ID do volume.
    uint32_t volume_id;

    //0x47 - byte 71
    // BS_VolLab: Rótulo do volume.
    char volume_label[11];
    
    //0x52 - byte 82
    // BS_FileSysType: Tipo de sistema de arquivos.  Defina  sempre  a  string  "FAT32  ".
    char file_system_type[8];

    //Se  considerarmos  o  conteúdo  do  setor  como  uma  matriz  de  bytes,  deve  ser  verdade  que  setor[510]  é  igual  a  0x55  e  setor[511]  é  igual  a  0xAA.
};
#pragma pack(pop)

bool readBPB(std::ifstream& img, BPB& bpb);
void printBPB(const BPB& bpb);

#endif