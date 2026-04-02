CC = gcc
CFLAGS = -ansi -pedantic -Wall -Werror -g
SRC_DIR = src

# Add all your object files here
OFILES = main.o game_logic.o file_reader.o evaluator.o tokenizer.o parser.o

nerdle: $(OFILES)
	$(CC) $(OFILES) -o nerdle

# Main needs access to game_logic and evaluator (for process_line)
main.o: $(SRC_DIR)/main.c $(SRC_DIR)/game_logic.h $(SRC_DIR)/evaluator.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c

game_logic.o: $(SRC_DIR)/game_logic.c $(SRC_DIR)/game_logic.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/game_logic.c

file_reader.o: $(SRC_DIR)/file_reader.c $(SRC_DIR)/file_reader.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/file_reader.c

# These are the new rules for your math engine
evaluator.o: $(SRC_DIR)/evaluator.c $(SRC_DIR)/evaluator.h $(SRC_DIR)/tokenizer.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/evaluator.c

tokenizer.o: $(SRC_DIR)/tokenizer.c $(SRC_DIR)/tokenizer.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/tokenizer.c

parser.o: $(SRC_DIR)/parser.c $(SRC_DIR)/parser.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/parser.c

.PHONY: clean
clean:
	rm -f *.o nerdle