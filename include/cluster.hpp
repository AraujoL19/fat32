// cluster.hpp
#ifndef CLUSTER_HPP
#define CLUSTER_HPP

#include "fat.hpp"
#include <fstream>

// Lê um cluster do disco (fat.part) para um DataCluster
void load_cluster(int cluster_index, DataCluster& cluster);

// Escreve um DataCluster no disco (fat.part) no índice de cluster fornecido
void write_cluster(int cluster_index, const DataCluster& cluster);

// Zera (limpa) um cluster no disco (útil para criar arquivos ou diretórios vazios)
void wipe_cluster(int cluster_index);

#endif // CLUSTER_HPP
