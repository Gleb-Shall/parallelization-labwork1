# BMP Image Processing Lab
# Author: Gleb Shikunov
# Student ID: st128274@student.spbu.ru

# Project name
PROJECT = bmp_processor

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -g -pthread
LDFLAGS = -pthread

# Parallel build support
MAKEFLAGS += -j$(shell nproc 2>/dev/null || echo 4)

# OpenMP support (check if compiler supports it)
OPENMP_FLAGS = -fopenmp
ifeq ($(shell $(CXX) -fopenmp -x c++ -c /dev/null -o /dev/null 2>/dev/null; echo $$?),0)
    CXXFLAGS += $(OPENMP_FLAGS)
    LDFLAGS += $(OPENMP_FLAGS)
else
    CXXFLAGS += -DNO_OPENMP
endif 

# Source files
SOURCES = main.cpp WorkWithBMP.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Optimized source files
OPTIMIZED_SOURCES = main_optimized.cpp WorkWithBMP_optimized.cpp
OPTIMIZED_OBJECTS = $(OPTIMIZED_SOURCES:.cpp=.o)
OPTIMIZED_TARGET = bmp_processor_optimized

# Test files removed

# Default target - clean and build
all: clean-all $(PROJECT)

# Clean all object files before building
clean-all:
	rm -f main.o WorkWithBMP.o main_optimized.o WorkWithBMP_optimized.o bmp_processor bmp_processor_optimized *.o *.exe *.dSYM
	@echo "Clean completed!"

# Build executable
$(PROJECT): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(PROJECT) $(LDFLAGS)
	@rm -f $(OBJECTS)
	@echo "Build completed successfully!"

# Build optimized executable
$(OPTIMIZED_TARGET): $(OPTIMIZED_OBJECTS)
	$(CXX) $(OPTIMIZED_OBJECTS) -o $(OPTIMIZED_TARGET) $(LDFLAGS)
	@rm -f $(OPTIMIZED_OBJECTS)
	@echo "Optimized build completed successfully!"

# Test executable removed

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

# Test targets removed

# Benchmark targets
benchmark: $(PROJECT)
	./$(PROJECT) --benchmark

benchmark-optimized: $(OPTIMIZED_TARGET)
	./$(OPTIMIZED_TARGET) --advanced

# Build both versions
build-all: clean-all $(PROJECT) $(OPTIMIZED_TARGET)
	@echo "Both versions built successfully!"

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
	@echo "  benchmark  - Run performance benchmarks"
	@echo "  benchmark-optimized - Run advanced scaling benchmarks"
	@echo "  build-all  - Build both standard and optimized versions"
	@echo "  memcheck   - Run with valgrind memory checking"
	@echo "  help       - Show this help message"

# Phony targets
.PHONY: all clean distclean run build-run rebuild debug release benchmark benchmark-optimized build-all memcheck install-deps help
