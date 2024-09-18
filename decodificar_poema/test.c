#include <stdio.h>
#include <stdlib.h>

int main()
{
    int buffer;
    int carac;
    char poema[340] = {0};

    FILE *file = fopen("file.bin", "r");


    for (int i = 0; i < 340; i++)
    {

        fread(&buffer, sizeof(int), 1, file);
        fread(&carac, sizeof(char), 1, file);

        poema[buffer] = carac;
    }

    fclose(file);

    printf("mensagem %s\n", poema);

    return 0;
}