#include "../include/shell.hpp"
#include "bpb.hpp"
#include "cluster.hpp"
#include "directory.hpp"

#include <vector>
#include <algorithm>


BPB bpb;

//TODO: init - inicializar o sistema de arquivos com as estruturas de dados, semelhante a formatar o sistema de arquivos virtual
void init() {
}

//TODO: load - carregar o sistema de arquivos do disco
void load(const std::string& imagePath) {
    std::ifstream img(imagePath, std::ios::binary);
    if (!img.is_open()) {
        std::cerr << "Erro ao abrir a imagem.\n";
        return;
    }

    if (!readBPB(img, bpb)) {
        std::cerr << "Erro ao ler o BPB.\n";
        return;
    }

    printBPB(bpb);
    img.close();
}
//TODO:  ls [/caminho/diretorio] - listar diretório
void ls(const std::string& imagePath) {
    std::ifstream img(imagePath, std::ios::binary);
    if (!img) {
        std::cerr << "Erro ao abrir imagem.\n";
        return;
    }

    if (!readBPB(img, bpb)) {
        std::cerr << "Erro ao ler BPB.\n";
        return;
    }

    uint32_t firstSector = FirstSectorOfCluster(bpb.BS_RootClus, bpb);
    uint32_t byteOffset = firstSector * bpb.BS_BytsPerSec;

    img.seekg(byteOffset, std::ios::beg);

    directory entry;
    while (img.read(reinterpret_cast<char*>(&entry), sizeof(directory))) {
        if (entry.DIR_Name[0] == 0x00) break; // fim das entradas
        if (entry.DIR_Name[0] == 0xE5) continue; // entrada livre
        if (entry.DIR_Attr == 0x0F) continue; // entrada de nome longo

        std::string name(entry.DIR_Name, 11);
        std::cout << name << (entry.DIR_Attr & 0x10 ? " <DIR>" : "") << "\n";
    }
}

//TODO: mkdir [/caminho/diretorio] - criar diretório
void mkdir() {
}

//TODO:  create [/caminho/arquivo] - criar arquivo
void create() {
}

//TODO: unlink  [/caminho/arquivo] - excluir arquivo ou diretório (o diretório precisa estar vazio)
void unlink() {
}

//TODO:  write "string" [/caminho/arquivo] - escrever dados em um arquivo (sobrescrever dados)
void write() {
}

//TODO: append "string" [/caminho/arquivo] - anexar dados em um arquivo
void append() {
}

//TODO: read [/caminho/arquivo] - ler o conteúdo de um arquivo
void read(const std::string& imagePath, const std::string& filename) {
    std::ifstream img(imagePath, std::ios::binary);
    if (!img) {
        std::cerr << "Erro ao abrir imagem.\n";
        return;
    }

    if (!readBPB(img, bpb)) {
        std::cerr << "Erro ao ler BPB.\n";
        return;
    }

    // Ir para o primeiro setor do diretório raiz
    uint32_t rootSector = FirstSectorOfCluster(bpb.BS_RootClus, bpb);
    img.seekg(rootSector * bpb.BS_BytsPerSec, std::ios::beg);

    directory entry;
    bool found = false;
    uint32_t firstCluster = 0;
    uint32_t fileSize = 0;

    // Procurar o arquivo pelo nome (sem path)
    while (img.read(reinterpret_cast<char*>(&entry), sizeof(directory))) {
        if (entry.DIR_Name[0] == 0x00) break;  // Fim das entradas
        if (entry.DIR_Name[0] == 0xE5 || entry.DIR_Attr == 0x0F) continue;  // Entrada livre ou nome longo

        std::string entryName(entry.DIR_Name, 11);
        std::string formattedName = filename;
        
        // Formatar nomes: FAT32 usa nomes em 11 caracteres (8+3), espaço preenchido
        formattedName.resize(11, ' ');

        if (entryName == formattedName) {
            found = true;
            firstCluster = (entry.DIR_FstClusHI << 16) | entry.DIR_FstClusLO;
            fileSize = entry.DIR_FileSize;
            break;
        }
    }

    if (!found) {
        std::cerr << "Arquivo \"" << filename << "\" não encontrado.\n";
        return;
    }

    // Ler os clusters do arquivo
    uint32_t cluster = firstCluster;
    uint32_t remainingBytes = fileSize;

    while (cluster >= 0x00000002 && cluster < 0x0FFFFFF8) { // Enquanto não for EOF
        uint32_t sector = FirstSectorOfCluster(cluster, bpb);
        uint32_t offset = sector * bpb.BS_BytsPerSec;
        uint32_t clusterSize = bpb.BS_SecPerClus * bpb.BS_BytsPerSec;
        uint32_t bytesToRead = std::min<uint32_t>(remainingBytes, clusterSize);


        std::vector<char> buffer(bytesToRead);
        img.seekg(offset, std::ios::beg);
        img.read(buffer.data(), bytesToRead);
        std::cout.write(buffer.data(), img.gcount());

        remainingBytes -= bytesToRead;

        // Agora precisamos ir ao próximo cluster na FAT
        uint32_t fatOffset = bpb.BS_RsvdSecCnt * bpb.BS_BytsPerSec + cluster * 4;
        img.seekg(fatOffset, std::ios::beg);
        img.read(reinterpret_cast<char*>(&cluster), sizeof(cluster));
        cluster &= 0x0FFFFFFF; // Mascara para os 28 bits válidos
    }

    std::cout << std::endl;
    img.close();
}