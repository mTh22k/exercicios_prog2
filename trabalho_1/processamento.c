#include <stdlib.h>
#include <math.h>
#include "LBP.h"

// Calcula o valor LBP para um pixel específico na imagem
unsigned char calcular_lbp(unsigned char *imagem, int largura, int altura, int x, int y)
{
    // Definindo os deslocamentos para os vizinhos
    int deslocamentos[8][2] = {{-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}};

    unsigned char centro = imagem[y * largura + x];
    unsigned char valorLBP = 0;
    int soma = 0;

    // Verifica os 8 vizinhos
    for (int i = 0; i < 8; i++)
    {
        int vizinhoX = x + deslocamentos[i][0];
        int vizinhoY = y + deslocamentos[i][1];

        // Verifica se o vizinho está dentro dos limites da imagem
        if (vizinhoX >= 0 && vizinhoX < largura && vizinhoY >= 0 && vizinhoY < altura)
        {
            unsigned char vizinho = imagem[vizinhoY * largura + vizinhoX];

            // Se o vizinho for maior ou igual ao centro, atualiza o somatório
            if (vizinho >= centro)
            {
                // Usa potência de 2 para calcular a posição do bit
                soma += pow(2, i);
            }
        }
    }

    // Armazena o valor final calculado
    valorLBP = soma;

    return valorLBP;
}

// Calcula o histograma LBP da imagem
void calcular_histograma(unsigned char *dadosImagem, double *histogramaLBP, int largura, int altura)
{
    int totalPixels = largura * altura;

    // Calcula o histograma LBP
    for (int y = 0; y < altura; y++)
    {
        for (int x = 0; x < largura; x++)
        {
            // Calcula o valor LBP para o pixel (x, y)
            unsigned char valorLBP = calcular_lbp(dadosImagem, largura, altura, x, y);
            histogramaLBP[valorLBP]++;
        }
    }

    // Normaliza o histograma
    for (int i = 0; i < LBP_VALUES; i++)
    {
        // Divide pela contagem total de pixels
        histogramaLBP[i] /= (double)totalPixels;
    }
}

// Calcula a distância Euclidiana entre dois histogramas
double distancia_euclidiana(double *hist1, double *hist2)
{
    double distancia = 0.0; // Inicializa a distância

    // Calcula a soma dos quadrados das diferenças
    for (int i = 0; i < LBP_VALUES; i++)
    {
        // Adiciona a diferença ao quadrado
        distancia += pow(hist1[i] - hist2[i], 2);
    }

    return sqrt(distancia);
}

// Processa uma imagem para calcular e salvar seu histograma LBP
void processar_imagem(const char *nomeArquivo, double *histogramaLBP)
{
    char nomeArquivoLBP[512];
    snprintf(nomeArquivoLBP, sizeof(nomeArquivoLBP), "%s.lbp", nomeArquivo);

    // Tenta carregar o histograma LBP do arquivo
    if (carregar_arquivo_lbp(nomeArquivoLBP, histogramaLBP))
    {
        return;
    }

    // Caso contrário, lê a imagem e calcula o histograma LBP
    FILE *arquivoEntrada = fopen(nomeArquivo, "rb");

    if (arquivoEntrada == NULL)
    {
        return;
    }

    char numeroMagico[2];
    int largura, altura, valorMaximo;

    // Lê o cabeçalho da imagem
    if (!ler_cabeçalho(arquivoEntrada, numeroMagico, &largura, &altura, &valorMaximo))
    {
        fclose(arquivoEntrada);
        return;
    }

    int totalPixels = largura * altura;

    // Aloca memória para os dados da imagem
    unsigned char *dadosImagem = (unsigned char *)malloc(totalPixels * sizeof(unsigned char));

    if (!dadosImagem)
    {
        fclose(arquivoEntrada);
        return;
    }

    // Lê a imagem dependendo do formato
    int sucesso = (numeroMagico[1] == '5') ? ler_p5(arquivoEntrada, dadosImagem, totalPixels) : ler_p2(arquivoEntrada, dadosImagem, totalPixels);

    if (sucesso)
    {
        calcular_histograma(dadosImagem, histogramaLBP, largura, altura);

        // Salva o histograma no arquivo
        salvar_arquivo_lbp(nomeArquivoLBP, histogramaLBP);
    }

    free(dadosImagem);
    fclose(arquivoEntrada);
}

void gerar_imagem_lbp(const char *nomeArquivoEntrada, const char *nomeArquivoSaida)
{

    FILE *arquivoEntrada = fopen(nomeArquivoEntrada, "rb");

    if (arquivoEntrada == NULL)
        return;

    char numeroMagico[2];
    int largura, altura, valorMaximo;

    // Lê o cabeçalho da imagem
    if (!ler_cabeçalho(arquivoEntrada, numeroMagico, &largura, &altura, &valorMaximo))
    {
        fclose(arquivoEntrada);
        return;
    }

    int totalPixels = largura * altura;
    unsigned char *dadosImagem = (unsigned char *)malloc(totalPixels * sizeof(unsigned char));
    unsigned char *imagemLBP = (unsigned char *)malloc(totalPixels * sizeof(unsigned char));

    // Lê a imagem dependendo do formato
    int sucesso = 0;
    if (numeroMagico[1] == '5')
    {
        sucesso = ler_p5(arquivoEntrada, dadosImagem, totalPixels);
    }
    else if (numeroMagico[1] == '2')
    {
        sucesso = ler_p2(arquivoEntrada, dadosImagem, totalPixels);
    }

    if (!sucesso)
    {
        free(dadosImagem);
        free(imagemLBP);
        fclose(arquivoEntrada);
        return;
    }

    // Calcula LBP para cada pixel e armazena na imagemLBP
    for (int y = 0; y < altura; y++)
    {
        for (int x = 0; x < largura; x++)
        {
            imagemLBP[y * largura + x] = calcular_lbp(dadosImagem, largura, altura, x, y);
        }
    }

    // Gera o arquivo de saída no mesmo formato do arquivo de entrada
    if (numeroMagico[1] == '5')
    {
        salvar_imagem_p5(nomeArquivoSaida, imagemLBP, largura, altura, valorMaximo);
    }
    else if (numeroMagico[1] == '2')
    {
        salvar_imagem_p2(nomeArquivoSaida, imagemLBP, largura, altura, valorMaximo);
    }

    free(dadosImagem);
    free(imagemLBP);
    fclose(arquivoEntrada);
}

// Salva a imagem LBP no formato P5
void salvar_imagem_p5(const char *nomeArquivo, unsigned char *dadosImagem, int largura, int altura, int valorMaximo)
{
    FILE *arquivoSaida = fopen(nomeArquivo, "wb");
    if (arquivoSaida == NULL)
        return;

    fprintf(arquivoSaida, "P5\n%d %d\n%d\n", largura, altura, valorMaximo);
    fwrite(dadosImagem, sizeof(unsigned char), largura * altura, arquivoSaida);

    fclose(arquivoSaida);
}

// Salva a imagem LBP no formato P2
void salvar_imagem_p2(const char *nomeArquivo, unsigned char *dadosImagem, int largura, int altura, int valorMaximo)
{
    FILE *arquivoSaida = fopen(nomeArquivo, "w");
    if (arquivoSaida == NULL)
        return;

    fprintf(arquivoSaida, "P2\n%d %d\n%d\n", largura, altura, valorMaximo);
    for (int i = 0; i < largura * altura; i++)
    {
        fprintf(arquivoSaida, "%d ", dadosImagem[i]);
    }

    fclose(arquivoSaida);
}
