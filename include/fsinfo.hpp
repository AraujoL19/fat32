//  recurso  para  armazenar  a  contagem  de  clusters  livres  "conhecida"  no  volume  FAT32,  para  que  ela  não  precise  ser  
// calculada  assim  que  uma  chamada  de  API  for  feita  para  perguntar  quanto  espaço  livre  há  no  volume  (como  no  final  de  
// uma  listagem  de  diretório).  O  número  do  setor  FSInfo  é  o  valor  no  campo  BPB_FSInfo;  para  sistemas  operacionais  
// Microsoft,  ele  é  sempre  definido  como  1.
#ifndef FSINFO_HPP
#define FSINFO_HPP
#include <cstdint>
#include <array>
#include "fat.hpp"

struct FSINFO {
    // FSI_LeadSig. Byte 0 a 3.
    // Valor  0x41615252.  Esta  assinatura  de  lead  é  usada  para  validar  que  este  é  de  fato  um  setor  FSInfo.
    static constexpr uint32_t FSI_LEAD_SIG = 0x41615252;
    
    // FSI_Reserved1. Byte 4 a 483.
    // Este  campo  está  atualmente  reservado  para  expansão  futura.  
    //O  código  no  formato  FAT32  deve  sempre  inicializar  todos  os  bytes  deste  campo  como  0.  
    //Bytes  neste  campo  nunca  devem  ser  usados.
    static constexpr std::array<uint8_t, 480> FSI_RESERVED1 = {0};
    
    // FSI_StrucSig. Byte 484 a 487.
    // Valor  0x61417272.  Esta  assinatura  de  estrutura  é  usada  para  validar  que  este  é  de  fato  um  setor  FSInfo.
    static constexpr uint32_t FSI_STRUC_SIG = 0x61417272;

    // FSI_Free_Count. Byte 488 a 491.
    // Este  campo  contém  o  número  de  clusters  livres  "conhecidos"  no  volume. 
    // Se  o  valor  for  0xFFFFFFFF,  a  contagem  livre  é  desconhecida  e  deve  ser  calculada.
    // Este  valor  é  atualizado  sempre  que  um  cluster  é  alocado  ou  liberado.
    static constexpr uint32_t FSI_FREE_COUNT = 0xFFFFFFFF; 

    // FSI_Nxt_Free. Byte 492 a 495.
    // Este  campo  contém  o  número  do  próximo  cluster  livre  "conhecido"  no  volume.
    // Normalmente,  esse  valor  é  definido  como  o  último  número  de  cluster  alocado  pelo  driver.  
    // Se  o  valor  for  0xFFFFFFFF,  não  há  dica  e  o  driver  deve  começar  a  procurar  pelo  cluster  2.
    static constexpr uint32_t FSI_NXT_FREE = 0xFFFFFFFF;

    // FSI_Reserved2. Byte 496 a 507.
    // Este  campo  está  atualmente  reservado  para  expansão  futura.  
    // O  código  no  formato  FAT32  deve  sempre  inicializar  todos  os  bytes  deste  campo  como  0.  
    // Bytes  neste  campo  nunca  devem  ser  usados.
    static constexpr std::array<uint8_t, 12> FSI_RESERVED2 = {0};

    // FSI_TrailSig. Byte 508 a 511.
    // Valor  0xAA550000.  Esta  assinatura  de  trilha  é  usada  para  validar  que  este  é  de  fato  um  setor  FSInfo.  
    // Observe  que  os  2  bytes  mais  altos  deste  valor  —  que  vão  para  os  bytes  nos  deslocamentos  510  e  511  —  correspondem  aos  bytes  de  assinatura  usados  nos  mesmos  deslocamentos  no  setor  0.
    static constexpr uint32_t FSI_TRAIL_SIG = 0xAA550000;
};

#endif // FSINFO_HPP