# FAT32

## Trabalho prático da disciplina de Sistemas Operacionais - UFJF

Grupo:

- Giovanni Almeida Dutra
- Juliana Hansen Basdão
- Lucas Araújo de Oliveira
- Mateus Lopes Felicio
- Thales Gomes Batista

Como base para o desenvolvimento foi utilizado um repositório base disponível em: [text](https://github.com/PUCRSOpenSource/tiny-shell/tree/master).

## FAT (File Allocation Table)

O FAT (File Allocation Table) organiza e gerencia como os arquivos são armazenados e recuperados, usando uma tabela para rastrear onde cada arquivo está localizado. 

O sistema FAT cria uma tabela (Tabela de Alocação de Arquivos) que armazena informações sobre a localização de cada arquivo, permitindo que o sistema operacional encontre e acesse os arquivos. 

- Onde os arquivos estão armazenados no disco;
- Quais blocos estão ocupados ou livres;
- Como navegar por arquivos fragmentados (que ocupam blocos não contíguos).

Essa tabela é essencialmente um vetor de inteiros, onde cada entrada aponta para o próximo bloco de um arquivo ou marca o fim do arquivo.

### **Estrutura do Sistema FAT32**

1. **Reserved Region (Região Reservada):**
    - Começa no setor 0 (Master Boot Record ou Volume Boot Record).
    - Inclui a BIOS Parameter Block (BPB), área de inicialização e cópias de segurança.
2. **FAT Region:**
    - Contém duas cópias da File Allocation Table.
    - A tabela FAT armazena a estrutura de alocação dos clusters, indicando quais estão livres, ocupados ou corrompidos.
3. **Data Region (Região de Dados):**
    - Onde ficam armazenados os arquivos e diretórios.
    - Começa a partir do primeiro cluster disponível.
    - Cada arquivo/diretório ocupa um ou mais clusters.

### Compilar e rodar
```
g++ -std=c++17 -Iinclude src/*.cpp -o build/fat32 && ./build/fat32
```