# Nome do compilador
CC = gcc

# Opções de compilação
CFLAGS = -Wall -Wextra -O2

# Nome do executável
TARGET = LBP

# Arquivos fonte (adicione todos os arquivos .c)
SRCS = LBP.c processamento.c leitura.c operações.c

# Objetos (serão gerados os .o a partir dos .c)
OBJS = $(SRCS:.c=.o)

# Regra padrão
all: $(TARGET)

# Regra para gerar o executável
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Regra para compilar arquivos .c em .o
%.o: %.c LBP.h
	$(CC) $(CFLAGS) -c $<

# Limpeza dos arquivos gerados
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
