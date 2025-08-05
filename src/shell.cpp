#include "../include/shell.hpp"
#include "bpb.hpp"

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
void ls() {
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