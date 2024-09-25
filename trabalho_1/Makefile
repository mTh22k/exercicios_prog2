# Nome do compilador
CC = gcc

# Opções de compilação
CFLAGS = -Wall -Wextra -O2

# Nome do executável
TARGET = lbp

# Arquivos fonte
SRCS = lbp.c  # Altere para o nome do seu arquivo fonte, se necessário

# Objetos
OBJS = $(SRCS:.c=.o)

# Regra padrão
all: $(TARGET)

# Regra para gerar o executável
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Regra para compilar arquivos .c em .o
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Limpeza dos arquivos gerados
clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
