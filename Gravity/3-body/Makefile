.PHONY: all build run clean

BUILD_DIR := build

all: build

build:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)

run: build
	./$(BUILD_DIR)/demo

clean:
	rm -rf $(BUILD_DIR)

