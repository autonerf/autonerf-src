# Project configuration
BUILD 		= build
SOURCE 		= src
INCLUDE 	= include
EXECUTABLE	= run

# Compiler settings
CC			= gcc
C_FLAGS		= -Wall -Werror -Wextra
LD_FLAGS	= -lv4l2
INC_FLAGS	= -I$(INCLUDE)

all: $(BUILD)/main.o $(BUILD)/camera.o
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -o $(EXECUTABLE) $(BUILD)/main.o $(BUILD)/camera.o

$(BUILD)/main.o: main.c $(INCLUDE)/camera.h $(BUILD)/camera.o
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/main.o main.c

$(BUILD)/camera.o: $(SOURCE)/camera.c $(INCLUDE)/camera.h
	$(CC) $(C_FLAGS) $(LD_FLAGS) $(INC_FLAGS) -c -o $(BUILD)/camera.o $(SOURCE)/camera.c

clean:
	rm -rf $(EXECUTABLE)
	rm -rf $(BUILD)/camera.o
	rm -rf $(BUILD)/main.o
