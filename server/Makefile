# Compiler and flags
CXX = g++
CXX_FLAGS = -std=c++11 -Wall -Wextra -pedantic

# Target executable
TARGET = server

# Source files
SRC = main.cpp

# Build the target
$(TARGET): $(SRC)
	$(CXX) $(CXX_FLAGS) -o $(TARGET) $(SRC)

# Clean the build
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: clean