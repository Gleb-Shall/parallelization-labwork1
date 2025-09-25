# BMP Image Processing Lab
# Author: Gleb Shikunov
# Student ID: st128274@student.spbu.ru

# Project name
PROJECT = bmp_processor

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g
LDFLAGS = 

# Source files
SOURCES = main.cpp WorkWithBMP.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Default target - clean and build
all: clean $(PROJECT)

# Build executable
$(PROJECT): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(PROJECT) $(LDFLAGS)
	@echo "Build completed successfully!"

# Compile object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(PROJECT) *.o *.exe *.dSYM
	@echo "Clean completed!"

# Deep clean - remove all generated files including processed images
distclean: clean
	rm -f *_rotated_*.bmp *_filtered_*.bmp
	@echo "Deep clean completed!"

# Run the program
run: $(PROJECT)
	./$(PROJECT)

# Build and run in one command
build-run: clean $(PROJECT)
	./$(PROJECT)

# Debug build
debug: CXXFLAGS += -DDEBUG -g3
debug: clean $(PROJECT)

# Release build
release: CXXFLAGS += -DNDEBUG -O3
release: clean $(PROJECT)

# Quick clean and rebuild
rebuild: clean $(PROJECT)
	@echo "Rebuild completed!"

# Check for memory leaks (requires valgrind)
memcheck: $(PROJECT)
	valgrind --leak-check=full --show-leak-kinds=all ./$(PROJECT)

# Install dependencies (if needed)
install-deps:
	@echo "No external dependencies required for this project"

# Help target
help:
	@echo "Available targets:"
	@echo "  all        - Clean and build the project (default)"
	@echo "  clean      - Remove build artifacts (object files, executable)"
	@echo "  distclean  - Remove all generated files including processed images"
	@echo "  run        - Build and run the program"
	@echo "  build-run  - Clean, build and run in one command"
	@echo "  rebuild    - Quick clean and rebuild"
	@echo "  debug      - Clean and build with debug symbols"
	@echo "  release    - Clean and build optimized release version"
	@echo "  memcheck   - Run with valgrind memory checking"
	@echo "  help       - Show this help message"

# Phony targets
.PHONY: all clean distclean run build-run rebuild debug release memcheck install-deps help
