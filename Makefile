CC = gcc
CPPFLAGS = -Isrc/ -D_POSIX_C_SOURCE=200809L
VPATH = src/
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99
USE_CLIMBING = 1

SRC = \
    src/eval.c \

BIN = pratt
OBJ = $(SRC:.c=.o)

.PHONY: all
all: $(BIN)

$(BIN):
$(BIN): $(OBJ) src/pratt.o

.PHONY: clean
clean:
	$(RM) $(OBJ) # remove object files
	$(RM) $(BIN) # remove main program
