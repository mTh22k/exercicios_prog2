#include <stdio.h>
#include <stdlib.h>
#include "LBP.h"

// Função para ler o cabeçalho de uma imagem PGM
int ler_cabeçalho(FILE *arquivoEntrada, char *numeroMagico, int *largura, int *altura, int *valorMax)
{
    ignorar_comentarios(arquivoEntrada);

    // Lê o número mágico (tipo da imagem)
    if (fscanf(arquivoEntrada, "%s", numeroMagico) != 1)
    {
        return 0;
    }

    // Verifica se o tipo da imagem é P5 ou P2
    if (numeroMagico[0] != 'P' || (numeroMagico[1] != '5' && numeroMagico[1] != '2'))
    {
        return 0;
    }

    ignorar_comentarios(arquivoEntrada);

    // Lê a largura e a altura da imagem
    if (fscanf(arquivoEntrada, "%d %d", largura, altura) != 2)
    {
        return 0;
    }

    ignorar_comentarios(arquivoEntrada);

    // Lê o valor máximo dos pixels
    if (fscanf(arquivoEntrada, "%d", valorMax) != 1)
    {
        return 0;
    }

    // Pula o caractere de nova linha após o cabeçalho
    fgetc(arquivoEntrada);

    ignorar_comentarios(arquivoEntrada);

    return 1;
}

void ignorar_comentarios(FILE *arquivo)
{
    int caractere;
    while ((caractere = fgetc(arquivo)) == '#')
    {
        // Se encontrar '#', ignora o resto da linha
        while ((caractere = fgetc(arquivo)) != '\n' && caractere != EOF)
            ;
    }
    // Devolve o caractere lido que não é '#', para continuar a leitura normal
    if (caractere != EOF)
    {
        ungetc(caractere, arquivo);
    }
}

int ler_p5(FILE *arquivoEntrada, unsigned char *dadosImagem, size_t totalPixels)
{
    size_t bytesLidos = fread(dadosImagem, sizeof(unsigned char), totalPixels, arquivoEntrada);
    if (bytesLidos != totalPixels)
    {
        return 0;
    }
    return 1;
}

void salvar_arquivo_lbp(const char *nomeArquivo, double *histogramaLBP)
{
    FILE *arquivo = fopen(nomeArquivo, "wb");
    if (arquivo != NULL)
    {
        fwrite(histogramaLBP, sizeof(double), LBP_VALUES, arquivo);
        fclose(arquivo);
    }
}

// Função para carregar o histograma LBP de um arquivo
int carregar_arquivo_lbp(const char *nomeArquivo, double *histogramaLBP)
{
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL)
    {
        return 0;
    }

    size_t contagemLida = fread(histogramaLBP, sizeof(double), LBP_VALUES, arquivo);
    fclose(arquivo);

    return contagemLida == LBP_VALUES;
}

int ler_p2(FILE *arquivoEntrada, unsigned char *dadosImagem, int totalPixels)
{
    for (int i = 0; i < totalPixels; i++)
    {
        int valorPixel;
        if (fscanf(arquivoEntrada, "%d", &valorPixel) != 1)
        {
            return 0;
        }
        // Armazena o valor do pixel
        dadosImagem[i] = (unsigned char)valorPixel;
    }
    return 1;
}
