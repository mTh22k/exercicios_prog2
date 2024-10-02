#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include "LBP.h"

// Função para listar imagens em um diretório com extensão .pgm
void listar_imagem_diretorio(const char *diretorio, char listaImagens[][256], int *contagem) {
    // Abre o diretório
    DIR *dir = opendir(diretorio);
    if (!dir) {
        return; // Retorna se não puder abrir o diretório
    }

    struct dirent *entrada;
    char caminhoArquivo[512]; // Caminho completo do arquivo
    struct stat estatArquivo;

    // Itera sobre o conteúdo do diretório
    while ((entrada = readdir(dir)) != NULL) {
        // Ignorar "." e ".."
        if (strcmp(entrada->d_name, ".") == 0 || strcmp(entrada->d_name, "..") == 0) {
            continue;
        }

        // Verifica se o arquivo tem a extensão .pgm
        if (strstr(entrada->d_name, ".pgm") == NULL || 
            strcmp(entrada->d_name + strlen(entrada->d_name) - 4, ".pgm") != 0) {
            continue; // Ignora se não for .pgm
        }

        // Constrói o caminho completo do arquivo
        snprintf(caminhoArquivo, sizeof(caminhoArquivo), "%s/%s", diretorio, entrada->d_name);

        // Verifica se é um arquivo regular
        if (stat(caminhoArquivo, &estatArquivo) == 0 && S_ISREG(estatArquivo.st_mode)) {
            // Adiciona o nome do arquivo à lista de imagens
            strcpy(listaImagens[*contagem], entrada->d_name);
            (*contagem)++; // Incrementa a contagem
        }
    }

    closedir(dir); // Fecha o diretório
}

// Função auxiliar para verificar se o arquivo existe
int verificar_arquivo_existe(const char *caminho) {
    struct stat buffer;
    return (stat(caminho, &buffer) == 0); // Retorna 1 se o arquivo existir, 0 se não
}

// Função auxiliar para extrair o nome do arquivo a partir do caminho completo
const char *extrair_nome_arquivo(const char *caminho) {
    const char *nomeArquivo = strrchr(caminho, '/');
    return (nomeArquivo) ? nomeArquivo + 1 : caminho; // Se encontrar '/', retorna o nome, senão retorna o caminho inteiro
}

// Função para encontrar a imagem mais similar a uma imagem de entrada
void achar_imagem_similar(const char *diretorio, const char *caminhoImagemEntrada) {
    // Verifica se a imagem de entrada existe
    if (!verificar_arquivo_existe(caminhoImagemEntrada)) {
        return; // Se a imagem não existe, retorna sem fazer nada
    }

    double histogramaLBPEntrada[LBP_VALUES] = {0}; // Histograma LBP da imagem de entrada
    processar_imagem(caminhoImagemEntrada, histogramaLBPEntrada); // Processa a imagem de entrada

    char listaImagens[100][256]; // Lista de imagens no diretório
    int contagemImagens = 0; // Contador de imagens
    listar_imagem_diretorio(diretorio, listaImagens, &contagemImagens); // Lista imagens no diretório

    const char *nomeImagemEntrada = extrair_nome_arquivo(caminhoImagemEntrada); // Extrai o nome da imagem de entrada

    double distanciaMinima = INFINITY; // Inicializa a distância mínima
    char *imagemMaisSimilar = NULL; // Ponteiro para a imagem mais similar

    // Compara a imagem de entrada com as imagens listadas
    for (int i = 0; i < contagemImagens; i++) {
        // Ignora a própria imagem de entrada
        if (strcmp(listaImagens[i], nomeImagemEntrada) == 0) {
            continue; // Pula a iteração se for a mesma imagem
        }

        char caminhoImagemReferencia[512]; // Caminho da imagem de referência
        // Verifica o comprimento do caminho
        int resultado = snprintf(caminhoImagemReferencia, sizeof(caminhoImagemReferencia), "%s/%s", diretorio, listaImagens[i]);

        // Corrige a comparação para evitar o aviso
        if (resultado < 0 || resultado >= (int)sizeof(caminhoImagemReferencia)) {
            fprintf(stderr, "Erro: Caminho da imagem de referência muito longo: %s/%s\n", diretorio, listaImagens[i]);
            continue; // Pula esta iteração se o caminho for muito longo
        }

        double histogramaLBPReferencia[LBP_VALUES] = {0}; // Histograma LBP da imagem de referência
        processar_imagem(caminhoImagemReferencia, histogramaLBPReferencia); // Processa a imagem de referência

        // Calcula a distância Euclidiana entre os histogramas LBP
        double distancia = distancia_euclidiana(histogramaLBPEntrada, histogramaLBPReferencia);

        // Atualiza a imagem mais similar se a distância for menor
        if (distancia < distanciaMinima) {
            distanciaMinima = distancia; // Atualiza a distância mínima
            imagemMaisSimilar = listaImagens[i]; // Atualiza a imagem mais similar
        }
    }

    // Exibe o resultado
    if (imagemMaisSimilar && distanciaMinima < INFINITY) {
        printf("Imagem mais similar: %s %f\n", imagemMaisSimilar, distanciaMinima);
    }
}



// Função para verificar a imagem de entrada e gerar uma imagem LBP
void verificar_pgm(const char *imagemEntrada, const char *imagemSaida) {
    // Verifica se a imagem de entrada tem a extensão .pgm
    if (strstr(imagemEntrada, ".pgm") == NULL) {
        return; // Retorna erro se não for .pgm
    }

    // Chama a função de geração de imagem LBP
    gerar_imagem_lbp(imagemEntrada, imagemSaida); // Passa o caminho correto
}
