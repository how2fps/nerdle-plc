CC = gcc
CFLAGS = -ansi -pedantic -Wall -Werror -g -I./include
SRC_DIR = src

# 1. Using OBJS consistently
OBJS = main.o game_logic.o game_ui.o file_reader.o evaluator.o tokenizer.o parser.o leaderboard.o

# 2. Changed $(OFILES) to $(OBJS)
nerdle: $(OBJS)
	$(CC) $(OBJS) -o nerdle

main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.c

game_logic.o: $(SRC_DIR)/game_logic.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/game_logic.c

game_ui.o: $(SRC_DIR)/game_ui.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/game_ui.c

file_reader.o: $(SRC_DIR)/file_reader.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/file_reader.c

evaluator.o: $(SRC_DIR)/evaluator.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/evaluator.c

tokenizer.o: $(SRC_DIR)/tokenizer.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/tokenizer.c

parser.o: $(SRC_DIR)/parser.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/parser.c

# 3. Added the missing leaderboard rule
leaderboard.o: $(SRC_DIR)/leaderboard.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/leaderboard.c

.PHONY: clean
clean:
	del *.o nerdle.exe