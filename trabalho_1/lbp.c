#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>

#define LBP_VALUES 256

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

int readPGMHeader(FILE *inputFile, char *magicNumber, int *width, int *height, int *maxVal) {
    if (fscanf(inputFile, "%s", magicNumber) != 1) {
        printf("Erro ao ler o cabeçalho.\n");
        return 0;
    }

    if (magicNumber[0] != 'P' || (magicNumber[1] != '5' && magicNumber[1] != '2')) {
        printf("Formato de arquivo PGM inválido: %s\n", magicNumber);
        return 0;
    }

    if (fscanf(inputFile, "%d %d", width, height) != 2) {
        printf("Erro ao ler largura e altura.\n");
        return 0;
    }

    if (fscanf(inputFile, "%d", maxVal) != 1) {
        printf("Erro ao ler o valor máximo de intensidade.\n");
        return 0;
    }

    fgetc(inputFile); // Pular o caractere de nova linha após o cabeçalho
    return 1;
}

int readImageDataP5(FILE *inputFile, unsigned char *imageData, size_t totalPixels) {
    size_t bytesRead = fread(imageData, sizeof(unsigned char), totalPixels, inputFile);
    if (bytesRead != totalPixels) {
        printf("Erro ao ler os dados da imagem.\n");
        return 0;
    }
    return 1; // Retorna 1 se a leitura for bem-sucedida
}

int readImageDataP2(FILE *inputFile, unsigned char *imageData, int totalPixels) {
    for (int i = 0; i < totalPixels; i++) {
        int pixelValue;
        if (fscanf(inputFile, "%d", &pixelValue) != 1) {
            printf("Erro ao ler os dados da imagem P2.\n");
            return 0;
        }
        imageData[i] = (unsigned char)pixelValue;
    }
    return 1;
}

void calculateLBPHistogram(unsigned char *imageData, double *lbpHistogram, int width, int height) {
    int totalPixels = width * height;

    // Calcula o histograma LBP
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            unsigned char lbpValue = calculateLBP(imageData, width, height, x, y);
            lbpHistogram[lbpValue]++;
        }
    }
    
    // Normaliza o histograma
    for (int i = 0; i < LBP_VALUES; i++) {
        lbpHistogram[i] /= (double)totalPixels;
    }
}

double calculateEuclideanDistance(double *hist1, double *hist2) {
    double distance = 0.0;
    for (int i = 0; i < LBP_VALUES; i++) {
        distance += pow(hist1[i] - hist2[i], 2);
    }
    return sqrt(distance);
}

void processImage(const char *fileName, double *lbpHistogram) {
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
    if (!imageData) {
        printf("Erro ao alocar memória para os dados da imagem.\n");
        fclose(inputFile);
        return;
    }

    int success = (magicNumber[1] == '5') ? readImageDataP5(inputFile, imageData, totalPixels)
                                          : readImageDataP2(inputFile, imageData, totalPixels);

    if (success) {
        calculateLBPHistogram(imageData, lbpHistogram, width, height);
    } else {
        printf("Erro ao processar a imagem %s.\n", fileName);
    }

    free(imageData);
    fclose(inputFile);
}

void listImagesInDirectory(const char *directory, char imageList[][256], int *count) {
    struct dirent *entry;
    DIR *dp = opendir(directory);
    if (dp == NULL) {
        printf("Erro ao abrir o diretório %s.\n", directory);
        return;
    }

    while ((entry = readdir(dp))) {
        if (entry->d_type == DT_REG) {
            strcpy(imageList[*count], entry->d_name);
            (*count)++;
        }
    }

    closedir(dp);
}

// Função para salvar a imagem LBP
void saveLBPImage(const char *outputFile, unsigned char *lbpImage, int width, int height) {
    FILE *file = fopen(outputFile, "wb");
    if (file == NULL) return;

    // Escreve o cabeçalho P5
    fprintf(file, "P5\n%d %d\n255\n", width, height);
    fwrite(lbpImage, sizeof(unsigned char), width * height, file);
    fclose(file);
}

// Função para gerar a imagem LBP
void generateLBPImage(const char *inputImageFile, const char *outputFile) {
    FILE *inputFile = fopen(inputImageFile, "rb");
    if (inputFile == NULL) return;

    char magicNumber[3];
    int width, height, maxVal;
    if (!readPGMHeader(inputFile, magicNumber, &width, &height, &maxVal)) {
        fclose(inputFile);
        return;
    }

    int totalPixels = width * height;
    unsigned char *imageData = (unsigned char *)malloc(totalPixels * sizeof(unsigned char));
    unsigned char *lbpImage = (unsigned char *)malloc(totalPixels * sizeof(unsigned char));

    if (magicNumber[1] == '5') {
        readImageDataP5(inputFile, imageData, totalPixels);
    } else if (magicNumber[1] == '2') {
        readImageDataP2(inputFile, imageData, totalPixels);
    }

    // Calcula LBP para cada pixel e armazena no lbpImage
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            lbpImage[y * width + x] = calculateLBP(imageData, width, height, x, y);
        }
    }

    saveLBPImage(outputFile, lbpImage, width, height);

    free(imageData);
    free(lbpImage);
    fclose(inputFile);
}

int main(int argc, char *argv[]) {
    if (argc == 5 && strcmp(argv[1], "-d") == 0 && strcmp(argv[3], "-i") == 0) {
        const char *directory = argv[2];
        const char *inputImageFile = argv[4];

        // Use double para armazenar o histograma de LBP
        double lbpHistogramReference[LBP_VALUES] = {0};  // Alterado para double
        processImage(inputImageFile, lbpHistogramReference);

        char imageList[100][256];
        int imageCount = 0;
        listImagesInDirectory(directory, imageList, &imageCount);

        double minDistance = INFINITY;
        char mostSimilarImage[256] = "";

        for (int i = 0; i < imageCount; i++) {
            char imagePath[4096];

            int written = snprintf(imagePath, sizeof(imagePath), "%s/%s", directory, imageList[i]);

            if (written < 0 || written >= (int)sizeof(imagePath)) {
                printf("Erro: caminho da imagem muito longo.\n");
                continue;
            }

            // Use double para armazenar o histograma LBP atual
            double lbpHistogramCurrent[LBP_VALUES] = {0};  // Alterado para double
            processImage(imagePath, lbpHistogramCurrent);

            double distance = calculateEuclideanDistance(lbpHistogramReference, lbpHistogramCurrent);

            if (distance < minDistance) {
                minDistance = distance;
                strcpy(mostSimilarImage, imageList[i]);
            }
        }

        printf("Imagem mais similar: %s %.6f\n", mostSimilarImage, minDistance);
    } else if (argc == 5 && strcmp(argv[1], "-i") == 0 && strcmp(argv[3], "-o") == 0) {
        const char *inputImageFile = argv[2];
        const char *outputImageFile = argv[4];
        generateLBPImage(inputImageFile, outputImageFile);
    } else {
        printf("Uso: ./lbp -d <diretório> -i <imagem> ou ./lbp -i <imagem> -o <imagem_out>\n");
        return 1;
    }

    return 0;
}

