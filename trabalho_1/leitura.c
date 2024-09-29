#include <stdio.h>
#include <stdlib.h>
#include "LBP.h"

// Função para ler o cabeçalho de uma imagem PGM
int ler_cabeçalho(FILE *arquivoEntrada, char *numeroMagico, int *largura, int *altura, int *valorMax) {
    // Lê o número mágico (tipo da imagem)
    if (fscanf(arquivoEntrada, "%s", numeroMagico) != 1) {
        return 0; // Retorna 0 se a leitura falhar
    }

    // Verifica se o tipo da imagem é P5 ou P2
    if (numeroMagico[0] != 'P' || (numeroMagico[1] != '5' && numeroMagico[1] != '2')) {
        return 0; // Retorna 0 se o tipo for inválido
    }

    // Lê a largura e a altura da imagem
    if (fscanf(arquivoEntrada, "%d %d", largura, altura) != 2) {
        return 0; // Retorna 0 se a leitura falhar
    }

    // Lê o valor máximo dos pixels
    if (fscanf(arquivoEntrada, "%d", valorMax) != 1) {
        return 0; // Retorna 0 se a leitura falhar
    }

    fgetc(arquivoEntrada); // Pula o caractere de nova linha após o cabeçalho
    return 1; // Retorna 1 se a leitura for bem-sucedida
}

// Função para ler a imagem no formato P5
int ler_p5(FILE *arquivoEntrada, unsigned char *dadosImagem, size_t totalPixels) {
    size_t bytesLidos = fread(dadosImagem, sizeof(unsigned char), totalPixels, arquivoEntrada);
    if (bytesLidos != totalPixels) {
        return 0; // Retorna 0 se a leitura falhar
    }
    return 1; // Retorna 1 se a leitura for bem-sucedida
}

// Função para salvar o histograma LBP em um arquivo
void salvar_arquivo_lbp(const char *nomeArquivo, double *histogramaLBP) {
    FILE *arquivo = fopen(nomeArquivo, "wb");
    if (arquivo != NULL) {
        fwrite(histogramaLBP, sizeof(double), LBP_VALUES, arquivo);
        fclose(arquivo); // Fecha o arquivo
    }
}

// Função para carregar o histograma LBP de um arquivo
int carregar_arquivo_lbp(const char *nomeArquivo, double *histogramaLBP) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL) {
        return 0; // Arquivo não encontrado
    }
    
    size_t contagemLida = fread(histogramaLBP, sizeof(double), LBP_VALUES, arquivo);
    fclose(arquivo);
    
    return contagemLida == LBP_VALUES; // Retorna 1 se a leitura foi bem-sucedida
}

// Função para ler a imagem no formato P2
int ler_p2(FILE *arquivoEntrada, unsigned char *dadosImagem, int totalPixels) {
    for (int i = 0; i < totalPixels; i++) {
        int valorPixel;
        if (fscanf(arquivoEntrada, "%d", &valorPixel) != 1) {
            return 0; // Retorna 0 se a leitura falhar
        }
        dadosImagem[i] = (unsigned char)valorPixel; // Armazena o valor do pixel
    }
    return 1; // Retorna 1 se a leitura for bem-sucedida
}

// Função para salvar a imagem LBP no formato PGM
void salvar_imagem_lbp(const char *arquivoSaida, unsigned char *imagemLBP, int largura, int altura) {
    FILE *arquivo = fopen(arquivoSaida, "wb");
    if (arquivo == NULL) return; // Retorna se não puder abrir o arquivo

    // Escreve o cabeçalho P5
    fprintf(arquivo, "P5\n%d %d\n255\n", largura, altura);
    fwrite(imagemLBP, sizeof(unsigned char), largura * altura, arquivo); // Escreve os dados da imagem
    fclose(arquivo); // Fecha o arquivo
}