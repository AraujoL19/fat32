#include "fat.hpp"

#include <iostream>
#include <string>
#include <stdlib.h>

#define BYTES_PER_SECTOR 512 //512 bytes, tamanho tradicional de um setor em sistemas de arquivos, menor bloco de leitura/escrita possível em discos
#define SECTORS_PER_CLUSTER 8 //4096 bytes por cluster (4 KB), que é o valor comum em sistemas FAT32
#define RESERVED_SECTORS 32 //setores reservados no início da partição para informações do sistema
#define NUM_FATS 2 //FAT32 mantém duas cópias da tabela de alocação para redundância e recuperação de erros
#define SECTORS_PER_FAT 64 //tamanho da FAT, Ex: 64 x 512 bytes = 32 KB por FAT
#define ROOT_DIR_CLUSTER 2 //por padrão, o primeiro cluster de dados disponível no diretório é o número 2
#define TOTAL_SECTORS 4096 //tamanho total do disco, Ex: 4096 x 512 B = 2 MB simulados

void FatSystem::init() {
    //TODO: verifica se o disco existe, inicializa (load) ou se não existir cria (create)
}

void FatSystem::create() {
    //TODO: cria 
}

void FatSystem::load() {
    //TODO: carrega 
}