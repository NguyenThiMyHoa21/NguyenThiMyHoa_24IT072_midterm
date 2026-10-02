CC = cc
CFLAGS = -Wall -Wextra -Werror -std=c11 -Iinclude

TARGET = ls

SRC = src/main.c \
      src/options.c \
      src/util.c \
      src/sort.c \
      src/format.c \
      src/list.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

src/%.o: src/%.c include/ls.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
