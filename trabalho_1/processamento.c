#include <stdlib.h>
#include <math.h>
#include "LBP.h"

// Calcula o valor LBP para um pixel específico na imagem
unsigned char calcular_lbp(unsigned char *imagem, int largura, int altura, int x, int y) {
    // Definindo os deslocamentos para os vizinhos
    int deslocamentos[8][2] = {{-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}};
    unsigned char centro = imagem[y * largura + x]; // Valor do pixel central
    unsigned char valorLBP = 0; // Inicializa o valor LBP como 0
    int somatorio = 0; // Variável para armazenar o valor final

    // Verifica os 8 vizinhos
    for (int i = 0; i < 8; i++) {
        int vizinhoX = x + deslocamentos[i][0]; // Coordenada x do vizinho
        int vizinhoY = y + deslocamentos[i][1]; // Coordenada y do vizinho

        // Verifica se o vizinho está dentro dos limites da imagem
        if (vizinhoX >= 0 && vizinhoX < largura && vizinhoY >= 0 && vizinhoY < altura) {
            unsigned char vizinho = imagem[vizinhoY * largura + vizinhoX]; // Valor do vizinho

            // Se o vizinho for maior ou igual ao centro, atualiza o somatório
            if (vizinho >= centro) {
                somatorio += pow(2, i); // Usa potência de 2 para calcular a posição do bit
            }
        }
    }

    valorLBP = somatorio; // Armazena o valor final calculado

    return valorLBP; // Retorna o valor LBP calculado
}

// Calcula o histograma LBP da imagem
void calcular_histograma(unsigned char *dadosImagem, double *histogramaLBP, int largura, int altura) {
    int totalPixels = largura * altura; // Total de pixels na imagem

    // Calcula o histograma LBP
    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            unsigned char valorLBP = calcular_lbp(dadosImagem, largura, altura, x, y); // Calcula o valor LBP para o pixel (x, y)
            histogramaLBP[valorLBP]++; // Incrementa a contagem no histograma
        }
    }
    
    // Normaliza o histograma
    for (int i = 0; i < LBP_VALUES; i++) {
        histogramaLBP[i] /= (double)totalPixels; // Divide pela contagem total de pixels
    }
}

// Calcula a distância Euclidiana entre dois histogramas
double distancia_euclidiana(double *hist1, double *hist2) {
    double distancia = 0.0; // Inicializa a distância
    // Calcula a soma dos quadrados das diferenças
    for (int i = 0; i < LBP_VALUES; i++) {
        distancia += pow(hist1[i] - hist2[i], 2); // Adiciona a diferença ao quadrado
    }
    return sqrt(distancia); // Retorna a raiz quadrada da soma
}

// Processa uma imagem para calcular e salvar seu histograma LBP
void processar_imagem(const char *nomeArquivo, double *histogramaLBP) {
    char nomeArquivoLBP[512];
    snprintf(nomeArquivoLBP, sizeof(nomeArquivoLBP), "%s.lbp", nomeArquivo); // Nome do arquivo LBP

    // Tenta carregar o histograma LBP do arquivo
    if (carregar_arquivo_lbp(nomeArquivoLBP, histogramaLBP)) {
        return; // Retorna se a leitura for bem-sucedida
    }

    // Caso contrário, lê a imagem e calcula o histograma LBP
    FILE *arquivoEntrada = fopen(nomeArquivo, "rb"); // Abre o arquivo de entrada em modo binário
    if (arquivoEntrada == NULL) {
        return; // Retorna se o arquivo não puder ser aberto
    }

    char numeroMagico[3]; // Para armazenar o número mágico do formato da imagem
    int largura, altura, valorMaximo; // Dimensões da imagem e valor máximo dos pixels

    // Lê o cabeçalho da imagem
    if (!ler_cabeçalho(arquivoEntrada, numeroMagico, &largura, &altura, &valorMaximo)) {
        fclose(arquivoEntrada); // Fecha o arquivo se a leitura falhar
        return;
    }

    int totalPixels = largura * altura; // Total de pixels na imagem
    unsigned char *dadosImagem = (unsigned char *)malloc(totalPixels * sizeof(unsigned char)); // Aloca memória para os dados da imagem
    if (!dadosImagem) {
        fclose(arquivoEntrada); // Fecha o arquivo se a alocação falhar
        return;
    }

    // Lê a imagem dependendo do formato
    int sucesso = (numeroMagico[1] == '5') ? ler_p5(arquivoEntrada, dadosImagem, totalPixels)
                                            : ler_p2(arquivoEntrada, dadosImagem, totalPixels);

    if (sucesso) {
        calcular_histograma(dadosImagem, histogramaLBP, largura, altura); // Calcula o histograma LBP
        salvar_arquivo_lbp(nomeArquivoLBP, histogramaLBP); // Salva o histograma no arquivo
    }

    free(dadosImagem); // Libera a memória alocada para os dados da imagem
    fclose(arquivoEntrada); // Fecha o arquivo de entrada
}

void gerar_imagem_lbp(const char *nomeArquivoEntrada, const char *nomeArquivoSaida) {
    // Abre o arquivo de entrada em modo binário
    FILE *arquivoEntrada = fopen(nomeArquivoEntrada, "rb");
    if (arquivoEntrada == NULL) return; // Retorna se o arquivo não puder ser aberto

    char numeroMagico[3]; // Para armazenar o número mágico do formato da imagem
    int largura, altura, valorMaximo; // Dimensões da imagem e valor máximo dos pixels
    
    // Lê o cabeçalho da imagem
    if (!ler_cabeçalho(arquivoEntrada, numeroMagico, &largura, &altura, &valorMaximo)) {
        fclose(arquivoEntrada); // Fecha o arquivo se a leitura falhar
        return;
    }

    int totalPixels = largura * altura; // Total de pixels na imagem
    unsigned char *dadosImagem = (unsigned char *)malloc(totalPixels * sizeof(unsigned char)); // Aloca memória para os dados da imagem
    unsigned char *imagemLBP = (unsigned char *)malloc(totalPixels * sizeof(unsigned char)); // Aloca memória para a imagem LBP

    // Lê a imagem dependendo do formato
    int sucesso = 0;
    if (numeroMagico[1] == '5') {
        sucesso = ler_p5(arquivoEntrada, dadosImagem, totalPixels); // Lê imagem P5
    } else if (numeroMagico[1] == '2') {
        sucesso = ler_p2(arquivoEntrada, dadosImagem, totalPixels); // Lê imagem P2
    }

    if (!sucesso) {
        free(dadosImagem);
        free(imagemLBP);
        fclose(arquivoEntrada);
        return; // Sai se a leitura falhar
    }

    // Calcula LBP para cada pixel e armazena na imagemLBP
    for (int y = 0; y < altura; y++) {
        for (int x = 0; x < largura; x++) {
            imagemLBP[y * largura + x] = calcular_lbp(dadosImagem, largura, altura, x, y); // Calcula LBP para o pixel
        }
    }

    // Gera o arquivo de saída no mesmo formato do arquivo de entrada
    if (numeroMagico[1] == '5') {
        salvar_imagem_p5(nomeArquivoSaida, imagemLBP, largura, altura, valorMaximo); // Salva como P5
    } else if (numeroMagico[1] == '2') {
        salvar_imagem_p2(nomeArquivoSaida, imagemLBP, largura, altura, valorMaximo); // Salva como P2
    }

    free(dadosImagem); // Libera a memória alocada para os dados da imagem
    free(imagemLBP); // Libera a memória alocada para a imagem LBP
    fclose(arquivoEntrada); // Fecha o arquivo de entrada
}


// Salva a imagem LBP no formato P5 (binário)
void salvar_imagem_p5(const char *nomeArquivo, unsigned char *dadosImagem, int largura, int altura, int valorMaximo) {
    FILE *arquivoSaida = fopen(nomeArquivo, "wb");
    if (arquivoSaida == NULL) return;

    fprintf(arquivoSaida, "P5\n%d %d\n%d\n", largura, altura, valorMaximo);
    fwrite(dadosImagem, sizeof(unsigned char), largura * altura, arquivoSaida);

    fclose(arquivoSaida);
}

// Salva a imagem LBP no formato P2 (texto)
void salvar_imagem_p2(const char *nomeArquivo, unsigned char *dadosImagem, int largura, int altura, int valorMaximo) {
    FILE *arquivoSaida = fopen(nomeArquivo, "w");
    if (arquivoSaida == NULL) return;

    fprintf(arquivoSaida, "P2\n%d %d\n%d\n", largura, altura, valorMaximo);
    for (int i = 0; i < largura * altura; i++) {
        fprintf(arquivoSaida, "%d ", dadosImagem[i]);
    }

    fclose(arquivoSaida);
}

