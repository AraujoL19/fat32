#include "../include/shell.hpp"
#include "bpb.hpp"
#include "cluster.hpp"
#include "directory.hpp"

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
void read() {
}