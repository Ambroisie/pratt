CC = gcc
CPPFLAGS = -Isrc/ -D_POSIX_C_SOURCE=200809L
CFLAGS = -Wall -Wextra -pedantic -Werror -std=c99
VPATH = src/ tests/
USE_CLIMBING = 1

SRC = \
    src/eval.c \

BIN = pratt
OBJ = $(SRC:.c=.o)

.PHONY: all
all: $(BIN)

$(BIN):
$(BIN): $(OBJ) src/pratt.o

check: testsuite
	./testsuite --verbose

TEST_SRC = \
    tests/testsuite.c \

TEST_OBJ = $(TEST_SRC:.c=.o)

testsuite: LDFLAGS+=-lcriterion -fsanitize=address
testsuite: CFLAGS+=-fsanitize=address
testsuite: $(OBJ) $(TEST_OBJ)

.PHONY: clean
clean:
	$(RM) $(OBJ) # remove object files
	$(RM) $(BIN) # remove main program
	$(RM) testsuite
