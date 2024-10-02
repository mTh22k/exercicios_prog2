#ifndef LBP_H
#define LBP_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

// Define o número de valores possíveis para LBP
#define LBP_VALUES 256

// Declaração de funções

// Calcula o valor LBP para um pixel específico
unsigned char calcular_lbp(unsigned char *imagem, int largura, int altura, int x, int y);

// Lê o cabeçalho de uma imagem PGM
int ler_cabeçalho(FILE *arquivoEntrada, char *numeroMagico, int *largura, int *altura, int *valorMax);

// Lê a imagem no formato P5 (binário)
int ler_p5(FILE *arquivoEntrada, unsigned char *dadosImagem, size_t totalPixels);

// Lê a imagem no formato P2 (texto)
int ler_p2(FILE *arquivoEntrada, unsigned char *dadosImagem, int totalPixels);

// Calcula o histograma LBP a partir dos dados da imagem
void calcular_histograma(unsigned char *dadosImagem, double *histogramaLBP, int largura, int altura);

// Calcula a distância Euclidiana entre dois histogramas
double distancia_euclidiana(double *hist1, double *hist2);

// Processa a imagem e calcula o histograma LBP
void processar_imagem(const char *nomeArquivo, double *histogramaLBP);

// auxilia a verificação do arquivo
int verificar_arquivo_existe(const char *caminho);

// auxilia a extrair o nome do arquivo a partir do caminho completo
const char *extrair_nome_arquivo(const char *caminho);

// Lista as imagens em um diretório
void listar_imagem_diretorio(const char *diretorio, char listaImagens[][256], int *contagem);

// Salva o histograma LBP em um arquivo
void salvar_arquivo_lbp(const char *nomeArquivo, double *histogramaLBP);

// Carrega o histograma LBP de um arquivo
int carregar_arquivo_lbp(const char *nomeArquivo, double *histogramaLBP);

// Salva a imagem LBP no formato PGM
void salvar_imagem_lbp(const char *arquivoSaida, unsigned char *imagemLBP, int largura, int altura);

// Gera uma imagem LBP a partir de uma imagem de entrada
void gerar_imagem_lbp(const char *imagemEntrada, const char *arquivoSaida);

// Encontra a imagem mais similar em um diretório
void achar_imagem_similar(const char *diretorio, const char *imagemEntrada);

// Verifica se a imagem de entrada está no diretório base e gera a imagem LBP
void verificar_pgm(const char *imagemEntrada, const char *arquivoSaida);

#endif
