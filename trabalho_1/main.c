#include <stdio.h>
#include <stdlib.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Função para calcular o valor LBP para um pixel
unsigned char calculateLBP(unsigned char *image, int width, int height, int x, int y) {
    int offsets[8][2] = {{-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}};
    unsigned char center = image[y * width + x];
    unsigned char lbpValue = 0;

    // Iterar pelos 8 vizinhos
    for (int i = 0; i < 8; i++) {
        int neighborX = x + offsets[i][0];
        int neighborY = y + offsets[i][1];

        // Certificar-se de que o vizinho está dentro dos limites da imagem
        if (neighborX >= 0 && neighborX < width && neighborY >= 0 && neighborY < height) {
            unsigned char neighbor = image[neighborY * width + neighborX];
            lbpValue |= (neighbor >= center) << i;  // Atribuir 1 ou 0 dependendo da comparação
        }
    }

    return lbpValue;
}

int main() {
    FILE *inputFile = fopen("Cecropia3.pgm", "rb");
    FILE *outputFile = fopen("output.pgm", "wb");
    FILE *lbpOutputFile = fopen("output_lbp.pgm", "wb");

    if (inputFile == NULL || outputFile == NULL || lbpOutputFile == NULL) {
        printf("Erro ao abrir os arquivos.\n");
        return 1;
    }

    char magicNumber[3];
    int width, height, maxVal;

    // Ler o cabeçalho da imagem PGM
    fscanf(inputFile, "%s", magicNumber);
    fscanf(inputFile, "%d %d", &width, &height);
    fscanf(inputFile, "%d", &maxVal);

    // Certificar-se de que está no formato correto
    if (magicNumber[0] != 'P' || magicNumber[1] != '5') {
        printf("Formato de arquivo PGM inválido.\n");
        return 1;
    }

    // Escrever o cabeçalho nos arquivos de saída
    fprintf(outputFile, "%s\n%d %d\n%d\n", magicNumber, width, height, maxVal);
    fprintf(lbpOutputFile, "%s\n%d %d\n%d\n", magicNumber, width, height, maxVal);

    // Alocar memória para armazenar os pixels da imagem
    unsigned char *imageData = (unsigned char *)malloc(width * height * sizeof(unsigned char));
    unsigned char *lbpImage = (unsigned char *)malloc(width * height * sizeof(unsigned char));

    // Pular o caractere de nova linha após o cabeçalho
    fgetc(inputFile);

    // Variáveis para verificar a sequência de pixels brancos
    int whiteCount = 0;
    int totalPixels = width * height;
    int i;

    // Ler os dados de pixel
    for (i = 0; i < totalPixels; i++) {
        fread(&imageData[i], sizeof(unsigned char), 1, inputFile);

        // Verificar se o pixel é branco (valor 255)
        if (imageData[i] == 255) {
            whiteCount++;
        } else {
            whiteCount = 0; // Resetar o contador se o pixel não for branco
        }

        // Se encontrar 3 pixels brancos consecutivos, parar a leitura
        if (whiteCount == 3) {
            printf("Três pixels brancos consecutivos encontrados. Parando a cópia da imagem.\n");
            break;
        }
    }

    // Escrever os dados de pixel no arquivo de saída até onde foi lido
    fwrite(imageData, sizeof(unsigned char), i - 2, outputFile);

    // Calcular o LBP para cada pixel e escrever no arquivo LBP
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            lbpImage[y * width + x] = calculateLBP(imageData, width, height, x, y);
        }
    }

    // Escrever os dados LBP no arquivo de saída
    fwrite(lbpImage, sizeof(unsigned char), width * height, lbpOutputFile);

    // Liberar a memória e fechar os arquivos
    free(imageData);
    free(lbpImage);
    fclose(inputFile);
    fclose(outputFile);
    fclose(lbpOutputFile);

    printf("Imagem copiada sem branco e LBP geradas com sucesso.\n");

    return 0;
}
