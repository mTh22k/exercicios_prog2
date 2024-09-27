#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

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
    DIR *dir = opendir(directory);
    if (!dir) {
        printf("Erro ao abrir o diretório %s.\n", directory);
        return;
    }

    struct dirent *entry;
    char filePath[512];
    struct stat fileStat;

    // Itera sobre o conteúdo do diretório
    while ((entry = readdir(dir)) != NULL) {
        // Ignorar "." e ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Verificar se o arquivo tem a extensão .pgm
        if (strstr(entry->d_name, ".pgm") == NULL || 
            strcmp(entry->d_name + strlen(entry->d_name) - 4, ".pgm") != 0) {
            continue;
        }

        // Construir o caminho completo do arquivo
        snprintf(filePath, sizeof(filePath), "%s/%s", directory, entry->d_name);

        // Verificar se é um arquivo regular
        if (stat(filePath, &fileStat) == 0 && S_ISREG(fileStat.st_mode)) {
            // Adicionar o nome do arquivo à lista de imagens
            strcpy(imageList[*count], entry->d_name);
            (*count)++;
        }
    }

    closedir(dir);
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

    // Aqui você pode usar a função saveLBPImage para salvar a imagem LBP no diretório base
    saveLBPImage(outputFile, lbpImage, width, height);

    free(imageData);
    free(lbpImage);
    fclose(inputFile);
}

int main(int argc, char *argv[]) {
    if (argc == 5 && strcmp(argv[1], "-d") == 0 && strcmp(argv[3], "-i") == 0) {
        const char *directory = argv[2];
        const char *inputImageFile = argv[4];

        // Construir o caminho completo da imagem de entrada
        char inputImagePath[512];
        snprintf(inputImagePath, sizeof(inputImagePath), "%s/%s", directory, inputImageFile);

        double inputLBPHistogram[LBP_VALUES] = {0};
        processImage(inputImagePath, inputLBPHistogram);

        char imageList[100][256]; // Lista de imagens no diretório
        int imageCount = 0;
        listImagesInDirectory(directory, imageList, &imageCount);

        double minDistance = INFINITY;
        char *mostSimilarImage = NULL;

        for (int i = 0; i < imageCount; i++) {
            // Ignorar a própria imagem de entrada
            if (strcmp(imageList[i], inputImageFile) == 0) {
                continue;
            }

            char referenceImagePath[512];
            snprintf(referenceImagePath, sizeof(referenceImagePath), "%s/%s", directory, imageList[i]);

            double referenceLBPHistogram[LBP_VALUES] = {0};
            processImage(referenceImagePath, referenceLBPHistogram);

            double distance = calculateEuclideanDistance(inputLBPHistogram, referenceLBPHistogram);

            if (distance < minDistance) {
                minDistance = distance;
                mostSimilarImage = imageList[i];
            }
        }

        if (mostSimilarImage) {
            printf("Imagem mais similar: %s\n", mostSimilarImage);
            printf("Distância Euclidiana: %f\n", minDistance);
        } else {
            printf("Nenhuma imagem similar encontrada.\n");
        }
    } else if (argc == 5 && strcmp(argv[1], "-i") == 0 && strcmp(argv[3], "-o") == 0) {
        const char *inputImageFile = argv[2];
        const char *outputFile = argv[4];

        // Verificar se a imagem de entrada está no diretório base
        char inputImagePath[512];
        snprintf(inputImagePath, sizeof(inputImagePath), "./base/%s", inputImageFile);

        // Construir o caminho completo para a saída no diretório /base
        char outputFilePath[512];
        snprintf(outputFilePath, sizeof(outputFilePath), "./base/%s", outputFile); // Saída também no diretório base

        // Verificar se a imagem de entrada tem a extensão .pgm
        if (strstr(inputImageFile, ".pgm") == NULL) {
            printf("A imagem de entrada deve ter a extensão .pgm\n");
            return 1; // Retorna erro
        }

        generateLBPImage(inputImagePath, outputFilePath); // Passa os caminhos completos
    } else {
        printf("Uso:\n");
        printf("  ./lbp -d <diretório> -i <imagem de entrada>\n");
        printf("  ./lbp -i <imagem de entrada> -o <imagem de saída>\n");
    }

    return 0;
}


