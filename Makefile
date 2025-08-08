#Makefile

SRC_DIR = src
BUILD_DIR = build
SOURCES = $(SRC_DIR)/shell.c $(SRC_DIR)/builtins.c $(SRC_DIR)/job.c $(SRC_DIR)/tokenize.c
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
TARGET = $(BUILD_DIR)/vtsh

CC = gcc
CFLAGS = -Wall -Wextra -O3 -std=c99
LDFLAGS = -lreadline

all: $(BUILD_DIR) $(TARGET)

$(TARGET): $(OBJECTS)
	@echo "Linking..."
	@$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	@echo "Done!"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	@echo "Creating /build directory"
	@mkdir -p $(BUILD_DIR)

clean:
	@echo "Cleaning..."
	rm -rf $(BUILD_DIR)
