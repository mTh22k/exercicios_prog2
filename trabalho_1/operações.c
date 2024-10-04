#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include "LBP.h"

// Função para listar imagens em um diretório com extensão .pgm
void listar_imagem_diretorio(const char *diretorio, char listaImagens[][256], int *contagem)
{
    // Abre o diretório
    DIR *dir = opendir(diretorio);
    if (!dir)
    {
        return;
    }

    struct dirent *entrada;

    // Caminho completo do arquivo
    char caminhoArquivo[512];
    struct stat estatArquivo;

    // Itera sobre o conteúdo do diretório
    while ((entrada = readdir(dir)) != NULL)
    {
        // Ignorar "." e ".."
        if (strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0)
        {
            continue;
        }

        // Verifica se o arquivo tem a extensão .pgm
        if (strstr(entrada->d_name, ".pgm") == NULL ||
            strcmp(entrada->d_name + strlen(entrada->d_name) - 4, ".pgm") != 0)
        {
            continue;
        }

        // Constrói o caminho completo do arquivo
        snprintf(caminhoArquivo, sizeof(caminhoArquivo), "%s/%s", diretorio, entrada->d_name);

        // Verifica se é um arquivo regular
        if (stat(caminhoArquivo, &estatArquivo) == 0 && S_ISREG(estatArquivo.st_mode))
        {
            // Adiciona o nome do arquivo à lista de imagens
            strcpy(listaImagens[*contagem], entrada->d_name);
            // Incrementa a contagem
            (*contagem)++;
        }
    }

    closedir(dir);
}

int verificar_arquivo_existe(const char *caminho)
{
    struct stat buffer;
    return (stat(caminho, &buffer) == 0);
}

const char *extrair_nome_arquivo(const char *caminho)
{
    const char *nomeArquivo = strrchr(caminho, '/');

    // Se encontrar '/', retorna o nome, senão retorna o caminho inteiro
    return (nomeArquivo) ? nomeArquivo + 1 : caminho;
}

void achar_imagem_similar(const char *diretorio, const char *caminhoImagemEntrada)
{
    // Verifica se a imagem de entrada existe
    if (!verificar_arquivo_existe(caminhoImagemEntrada))
    {
        return;
    }

    // Histograma LBP da imagem de entrada
    double histogramaLBPEntrada[LBP_VALUES] = {0};
    processar_imagem(caminhoImagemEntrada, histogramaLBPEntrada);

    // Verifica se a imagem de entrada foi processada corretamente
    // Supondo que o histograma seja um array de zeros se falhar
    if (histogramaLBPEntrada[0] == 0 && LBP_VALUES > 1) 
    {
        return; 
    }

    // Lista de imagens no diretório
    char listaImagens[100][256];
    int contagemImagens = 0;
    listar_imagem_diretorio(diretorio, listaImagens, &contagemImagens);

    const char *nomeImagemEntrada = extrair_nome_arquivo(caminhoImagemEntrada);

    double distanciaMinima = INFINITY;
    char *imagemMaisSimilar = NULL;

    // Compara a imagem de entrada com as imagens listadas
    for (int i = 0; i < contagemImagens; i++)
    {
        // Ignora a própria imagem de entrada
        if (strcmp(listaImagens[i], nomeImagemEntrada) == 0)
        {
            continue;
        }

        // Caminho da imagem de referência
        char caminhoImagemReferencia[512];

        // Verifica o comprimento do caminho
        int resultado = snprintf(caminhoImagemReferencia, sizeof(caminhoImagemReferencia), "%s/%s", diretorio, listaImagens[i]);

        // Corrige a comparação para evitar o aviso
        if (resultado < 0 || resultado >= (int)sizeof(caminhoImagemReferencia))
        {
            continue;
        }

        double histogramaLBPReferencia[LBP_VALUES] = {0};
        processar_imagem(caminhoImagemReferencia, histogramaLBPReferencia);

        // Verifica se a imagem de referência foi processada corretamente
        // Supondo que o histograma seja um array de zeros se falhar
        if (histogramaLBPReferencia[0] == 0 && LBP_VALUES > 1) 
        {
            continue; // Ignora a imagem de referência se não puder ser processada
        }

        // Calcula a distância Euclidiana entre os histogramas LBP
        double distancia = distancia_euclidiana(histogramaLBPEntrada, histogramaLBPReferencia);

        // Atualiza a imagem mais similar se a distância for menor
        if (distancia < distanciaMinima)
        {
            // Atualiza a distância mínima
            distanciaMinima = distancia;

            // Atualiza a imagem mais similar
            imagemMaisSimilar = listaImagens[i];
        }
    }

    // Exibe o resultado
    if (imagemMaisSimilar && distanciaMinima < INFINITY)
    {
        printf("Imagem mais similar: %s %f\n", imagemMaisSimilar, distanciaMinima);
    }
}

// Função para verificar a imagem de entrada e gerar uma imagem LBP
void verificar_pgm(const char *imagemEntrada, const char *imagemSaida)
{
    // Verifica se a imagem de entrada tem a extensão .pgm
    if (strstr(imagemEntrada, ".pgm") == NULL)
    {
        return;
    }

    // Chama a função de geração de imagem LBP
    gerar_imagem_lbp(imagemEntrada, imagemSaida);
}
