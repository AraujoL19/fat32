// Um  diretório  FAT  nada  mais  é  do  que  um  "arquivo"  composto  por  uma  lista  linear  de  estruturas  de  32  bytes.  
// O  único  diretório  especial,  que  deve  estar  sempre  presente,  é  o  diretório  raiz.
// Para  FAT32,  o  diretório  raiz  pode  ter  tamanho  variável  e  é  uma  cadeia  de  clusters,  assim  como  qualquer  outro  diretório.  
// O  primeiro  cluster  do  diretório  raiz  em  um  volume  FAT32  é  armazenado  em  BPB_RootClus.
// Ao  contrário  de  outros  diretórios,  o  diretório  raiz  em  si,  em  qualquer  tipo  de  FAT,  não  possui  nenhum  registro  de  data  ou  hora,  
// não  possui  um  nome  de  arquivo  (além  do  nome  implícito  "\")  e  não  contém  os  arquivos  "."  e  ".."  como  as  duas  primeiras  entradas  de  diretório.

#include <cstdint>
#ifndef char DIR_Name[11];DIRECTORY_HPP
#define DIRECTORY_HPP  

struct directory
{
    // DIR_Name. Byte 0 a 10.
    // Se  DIR_Name[0]  ==  0xE5,  então  a  entrada  do  diretório  está  livre  (não  há  nome  de  arquivo  ou  diretório  nesta entrada).
    // Se  DIR_Name[0]  ==  0x00,  então  a  entrada  do  diretório  está  vazia  (não  há  mais entradas no diretório).
    //  Se  DIR_Name[0]  ==  0x05,  então  o  caractere  de  nome  de  arquivo  real  para  este  byte  é  0xE5.  O  valor  especial  0x05  é  usado  para  um  caso  especial  de  nome  de  arquivo  kanji  para  o  Japão. 
    // O  campo  DIR_Name  é,  na  verdade,  dividido  em  duas  partes:  a  parte  principal  do  nome,  com  8  caracteres,  e  a  extensão,  com  3  caracteres.  Essas  duas  partes  são  preenchidas  com  um  "espaço  final"  com  bytes  de  0x20.
    // Há  um  caractere  '.'  implícito  entre  a  parte  principal  do  nome  e  a  parte  da  extensão  do  nome  que  não  está  presente  em  DIR_Name.
    // Caracteres  minúsculos  não  são  permitidos  em  DIR_Name.
    // Em  diretórios  FAT,  todos  os  nomes  são  únicos.
    char DIR_Name[11];

    // DIR_Attr. Byte 11.
    //  DIR_Attr  especifica  atributos  do  arquivo:
    // 0x01 - ATTR_READ_ONLY - O arquivo é somente leitura. Gravações não são permitidas.
    // 0x02 - ATTR_HIDDEN - O arquivo está oculto. Não é exibido em listagens normais.
    // 0x04 - ATTR_SYSTEM - É um arquivo do sistema operacional. Não deve ser modificado ou excluído.
    // 0x08 - ATTR_VOLUME_ID - O arquivo é um identificador de volume. Deve  haver  apenas  um  "arquivo"  no  volume  com  este  atributo  definido,  e  esse  arquivo  deve  estar  no  diretório  raiz.
    // 0x10 - ATTR_DIRECTORY - Indica  que  este  arquivo  é  na  verdade  um  contêiner  para  outros  arquivos.
    // 0x20 - ATTR_ARCHIVE - Este  atributo  oferece  suporte  a  utilitários  de  backup.  
                          // Este  bit  é  definido  pelo  driver  do  sistema  de  arquivos  FAT  quando  um  arquivo  é  criado,  renomeado  ou  gravado.  
                          // Utilitários  de  backup  podem  usar  este  atributo  para  indicar  quais  arquivos  no  volume  foram  modificados  desde  a  última  vez  que  um  backup  foi  realizado.
    // 0x40 - ATTR_LONG_NAME -  a  combinação  de  bits  do  atributo  ATTR_LONG_NAME  indica  que  o  "arquivo"  é,  na  verdade,  parte  da  entrada  de  nome  longo  de  algum  outro  arquivo.
    // Os  dois  bits  superiores  do  byte  de  atributo  são  reservados  e  sempre  devem  ser  definidos  como  0  quando  um  arquivo  é  criado  e  nunca  modificados  ou  consultados  depois  disso.
    uint8_t DIR_Attr;

    // DIR_NTRes. Byte 12. 
    // Reservado  para  uso  no  Windows  NT.  Defina  o  valor  como  0  quando  um  arquivo  for  criado  e  nunca  mais  o  modifique  ou  visualize  depois  disso.
    uint8_t DIR_NTRes;

    // DIR_CrtTimeTenth. Byte 13.
    // Carimbo  de  milissegundos  no  momento  da  criação  do  arquivo
    uint8_t DIR_CrtTimeTenth;

    // DIR_CrtTime. Byte 14 a 15.
    // Hora  em  que  o  arquivo  foi  criado.
    uint16_t DIR_CrtTime;

    // DIR_CrtDate. Byte 16 a 17.
    // Data  em  que  o  arquivo  foi  criado.
    uint16_t DIR_CrtDate;

    // DIR_LstAccDate. Byte 18 a 19.
    // Data  do  último  acesso  ao  arquivo.
    uint16_t DIR_LstAccDate;

    // DIR_FstClusHI. Byte 20 a 21.
    // Palavra  alta  do  primeiro  número  de  cluster  desta  entrada.
    uint16_t DIR_FstClusHI;

    // DIR_WrtTime. Byte 22 a 23.
    // Horário  da  última  gravação.
    uint16_t DIR_WrtTime;

    // DIR_WrtDate. Byte 24 a 25.
    // Data  da  última  gravação.
    uint16_t DIR_WrtDate;

    // DIR_FstClusLO. Byte 26 a 27.
    // Palavra  baixa  do  primeiro  número  de  cluster  desta  entrada.
    uint16_t DIR_FstClusLO;

    // DIR_FileSize. Byte 28 a 31.
    // DWORD  de  32  bits  contendo  o  tamanho  deste  arquivo  em  bytes.
    uint32_t DIR_FileSize;
};

/*  Quando  um  diretório  é  criado,  um  arquivo  com  o  bit  ATTR_DIRECTORY  definido  em  seu  campo  DIR_Attr,  você  define  seu  
DIR_FileSize  como  0.  DIR_FileSize  não  é  usado  e  é  sempre  0  em  um  arquivo  com  o  atributo  ATTR_DIRECTORY.  
Um  cluster  é  alocado  ao  diretório  e  você  define  DIR_FstClusLO  e  DIR_FstClusHI  para  esse  número  de  cluster  e  coloca  uma  marca  EOC  na  entrada  desse  cluster  na  FAT.  
Em  seguida,  você  inicializa  todos  os  bytes  desse  cluster  como  0.  Se  o  diretório  for  o  diretório  raiz,  está  pronto.  
Se  o  diretório  não  for  o  diretório  raiz,  você  precisa  criar  duas  entradas  especiais  nos  dois  primeiros  arquivos  de  32  bytes.

 Formato  de  Data:
  Um  carimbo  de  data  de  entrada  de  diretório  FAT  é  um  campo  de  16  bits  que  é  basicamente  uma  data  relativa  à  era  
 MS-DOS  de  01/01/1980.  Aqui  está  o  formato  (bit  0  é  o  LSB  da  palavra  de  16  bits,  bit  15  é  o  MSB  da  palavra  de  16  bits):
 Bits  0–4:  Dia  do  mês,  intervalo  de  valores  válidos  de  1  a  31,  inclusive.
 Bits  5–8:  Mês  do  ano,  1  =  janeiro,  intervalo  de  valores  válidos  de  1  a  12,  inclusive.
 Bits  9–15:  Contagem  de  anos  a  partir  de  1980,  intervalo  de  valores  válidos  de  0  a  127  inclusive  (1980–2107).
 
 Formato  de  hora:
 Um  registro  de  data  e  hora  de  entrada  de  diretório  FAT  é  um  campo  de  16  bits  com  granularidade  de  2  segundos.
 Aqui  está  o  formato  (o  bit  0  é  o  LSB  da  palavra  de  16  bits,  o  bit  15  é  o  MSB  da  palavra  de  16  bits).
 Bits  0–4:  contagem  de  2  segundos,  intervalo  de  valores  válidos  de  0  a  29  inclusive  (0  a  58  segundos).
 Bits  5–10:  Minutos,  intervalo  de  valores  válidos  de  0  a  59,  inclusive.
 Bits  11–15:  Horas,  intervalo  de  valores  válidos  de  0  a  23,  inclusive.*/


#endif // DIRECTORY_HPP