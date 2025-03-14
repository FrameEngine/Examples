# Usage:
#   make build TARGET_DIR=path/to/demo
#   make run TARGET_DIR=path/to/demo
#   make clean TARGET_DIR=path/to/demo
#

ifndef TARGET_DIR
$(error TARGET_DIR is not set. Usage: make build TARGET_DIR=path/to/demo)
endif

.PHONY: build run clean

build:
	@echo "Building demo project in $(TARGET_DIR)..."
	$(MAKE) -C $(TARGET_DIR)

run:
	@echo "Running demo project in $(TARGET_DIR)..."
	$(MAKE) -C $(TARGET_DIR) run

clean:
	@echo "Cleaning demo project in $(TARGET_DIR)..."
	$(MAKE) -C $(TARGET_DIR) clean

