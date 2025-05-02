# Compiler and flags
CXX = g++
CXX_FLAGS = -std=c++11 -Wall -Wextra -pedantic

# Target executable
TARGET = client

# Source files
SRC = main.cpp

# Build the target
$(TARGET): $(SRC)
	$(CXX) $(CXX_FLAGS) -o $(TARGET) $(SRC)

# Clean the build
build:
	rm -f $(TARGET)
	make

# Phony targets
.PHONY: build