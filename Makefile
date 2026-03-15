CC = gcc
CFLAGS = -ansi -pedantic -Wall -Werror -g
SRC_DIR=src
OFILES = main.o game_logic.o

nerdle: $(OFILES)
	$(CC) $(OFILES) -o nerdle

main.o: $(SRC_DIR)/main.c $(SRC_DIR)/game_logic.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c

game_logic.o: $(SRC_DIR)/game_logic.c $(SRC_DIR)/game_logic.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/game_logic.c

.PHONY: clean
clean: rm -f *.o nerdle