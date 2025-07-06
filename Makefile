#Makefile

SRC_DIR = src
BUILD_DIR = build
SRC = $(SRC_DIR)/main.c
TARGET = $(BUILD_DIR)/vtsh

CC = gcc
CFLAGS = -lreadline -Wall -Wextra -O3

all: $(BUILD_DIR) $(TARGET)

$(TARGET): $(SRC)
	@echo "Compiling..."
	@$(CC) $(CFLAGS) $< -o $@
	@echo "Done!"

$(BUILD_DIR):
	@echo "Creating /build directory"
	@mkdir -p $(BUILD_DIR)

clean:
	@echo "Cleaning..."
	@rm -rf $(BUILD_DIR)
