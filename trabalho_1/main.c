#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define LBP_VALUES 256
#define MAX_IMAGES 7 // Número máximo de imagens que você deseja processar

// Função para calcular o valor LBP para um pixel
unsigned char calculateLBP(unsigned char *image, int width, int height, int x, int y) {
    int offsets[8][2] = {{-1, -1}, {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}};
    unsigned char center = image[y * width + x];
    unsigned char lbpValue = 0;

    for (int i = 0; i < 8; i++) {
        int neighborX = x + offsets[i][0];
        int neighborY = y + offsets[i][1];

        if (neighborX >= 0 && neighborX < width && neighborY >= 0 && neighborY < height) {
            unsigned char neighbor = image[neighborY * width + neighborX];
            lbpValue |= (neighbor >= center) << i;  
        }
    }

    return lbpValue;
}

// Função para ler o cabeçalho da imagem PGM
int readPGMHeader(FILE *inputFile, char *magicNumber, int *width, int *height, int *maxVal) {
    fscanf(inputFile, "%s", magicNumber);
    fscanf(inputFile, "%d %d", width, height);
    fscanf(inputFile, "%d", maxVal);

    if (magicNumber[0] != 'P' || (magicNumber[1] != '2' && magicNumber[1] != '5')) {
        printf("Formato de arquivo PGM inválido.\n");
        return 0;
    }

    fgetc(inputFile); // Pular o caractere de nova linha após o cabeçalho
    return 1;
}

// Função para ler os dados de pixel da imagem P2 (ASCII)
int readImageDataP2(FILE *inputFile, unsigned char *imageData, int totalPixels) {
    for (int i = 0; i < totalPixels; i++) {
        int pixelValue;
        fscanf(inputFile, "%d", &pixelValue);
        imageData[i] = (unsigned char)pixelValue;
    }
    return totalPixels;
}

// Função para ler os dados de pixel da imagem P5 (binário)
int readImageDataP5(FILE *inputFile, unsigned char *imageData, int totalPixels) {
    fread(imageData, sizeof(unsigned char), totalPixels, inputFile);
    return totalPixels;
}

// Função para calcular a imagem LBP e o histograma LBP
void calculateLBPHistogram(unsigned char *imageData, int *lbpHistogram, int width, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned char lbpValue = calculateLBP(imageData, width, height, x, y);
            lbpHistogram[lbpValue]++;
        }
    }
}

// Função para calcular a distância Euclidiana entre dois histogramas
double calculateEuclideanDistance(int *hist1, int *hist2) {
    double distance = 0.0;

    for (int i = 0; i < LBP_VALUES; i++) {
        distance += pow(hist1[i] - hist2[i], 2);
    }

    return sqrt(distance);
}

// Função principal para processar uma imagem e retornar o histograma LBP
void processImage(const char *fileName, int *lbpHistogram) {
    FILE *inputFile = fopen(fileName, "rb");

    if (inputFile == NULL) {
        printf("Erro ao abrir o arquivo %s.\n", fileName);
        return;
    }

    char magicNumber[3];
    int width, height, maxVal;

    if (!readPGMHeader(inputFile, magicNumber, &width, &height, &maxVal)) {
        fclose(inputFile);
        return;
    }

    int totalPixels = width * height;
    unsigned char *imageData = (unsigned char *)malloc(totalPixels * sizeof(unsigned char));

    if (magicNumber[1] == '2') { // P2 (ASCII)
        readImageDataP2(inputFile, imageData, totalPixels);
    } else if (magicNumber[1] == '5') { // P5 (binário)
        readImageDataP5(inputFile, imageData, totalPixels);
    }

    calculateLBPHistogram(imageData, lbpHistogram, width, height);

    free(imageData);
    fclose(inputFile);
}

int main() {
    // Declaração de arrays sem inicialização
    char *imageFiles[MAX_IMAGES] = {
        "Cecropia3.pgm",
        "Cecropia2.pgm",
        "Combretum2.pgm",
        "Combretum1.pgm",
        "Croton2.pgm",
        "Croton1.pgm",
        "Apuleia1.pgm"
        // Adicione mais arquivos aqui, até MAX_IMAGES
    };

    // Array para os histogramas LBP
    int lbpHistograms[MAX_IMAGES][LBP_VALUES];
    memset(lbpHistograms, 0, sizeof(lbpHistograms)); // Inicializa o array com zeros

    // Processar cada imagem e calcular os histogramas
    for (int i = 0; i < MAX_IMAGES; i++) {
        processImage(imageFiles[i], lbpHistograms[i]);
    }

    // Calcular as distâncias Euclidianas
    double minDistance = INFINITY;
    int mostSimilarIndex = -1;

    for (int i = 1; i < MAX_IMAGES; i++) {
        double distance = calculateEuclideanDistance(lbpHistograms[0], lbpHistograms[i]);
        printf("Distância entre Cecropia3.pgm e %s: %.2f\n", imageFiles[i], distance);

        if (distance < minDistance) {
            minDistance = distance;
            mostSimilarIndex = i;
        }
    }

    // Determinar qual imagem é mais parecida com Cecropia3
    if (mostSimilarIndex != -1) {
        printf("A imagem %s é a mais semelhante à Cecropia3.pgm (Distância: %.2f).\n", imageFiles[mostSimilarIndex], minDistance);
    }

    return 0;
}
