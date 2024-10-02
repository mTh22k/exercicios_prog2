#include <stdio.h>
#include <string.h>
#include "LBP.h"

int main(int argc, char *argv[]) {
    // Verifica se o número de argumentos está correto e se as opções -d e -i foram usadas
    if (argc == 5 && strcmp(argv[1], "-d") == 0 && strcmp(argv[3], "-i") == 0) {
        // Armazena o diretório e a imagem de entrada a partir dos argumentos
        const char *diretorio = argv[2];
        const char *imagemEntrada = argv[4];

        // Chama a função para encontrar a imagem mais similar
        achar_imagem_similar(diretorio, imagemEntrada);

    // Verifica se as opções -i e -o foram usadas
    } else if (argc == 5 && strcmp(argv[1], "-i") == 0 && strcmp(argv[3], "-o") == 0) {
        // Armazena a imagem de entrada e a imagem de saída a partir dos argumentos
        const char *imagemEntrada = argv[2];
        const char *imagemSaida = argv[4];

        // Chama a função para verificar a imagem de entrada e gerar a imagem LBP
        verificar_pgm(imagemEntrada, imagemSaida);
    }

    return 0; // Retorna 0 para indicar que o programa terminou com sucesso
}
