/*
   Optimized BMP Image Processing with Advanced Parallelization
   Author: Gleb Shikunov
   Student ID: st128274@student.spbu.ru
   Lab1 - BMP Image Processing
*/

#ifndef WORKWITHBMP_OPTIMIZED_H
#define WORKWITHBMP_OPTIMIZED_H

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <memory>
#include <thread>
#include <future>
#include <mutex>
#include <atomic>
#include <chrono>
#include <random>
#ifdef _OPENMP
#include <omp.h>
#elif defined(NO_OPENMP)
// OpenMP not available, use std::thread fallback
#endif

class BMPImageOptimized {
private:
    // BMP file structure constants
    static constexpr int FILE_HEADER_SIZE = 14;
    static constexpr int INFO_HEADER_SIZE = 40;
    
    // File headers
    unsigned char fileHeader[FILE_HEADER_SIZE];
    unsigned char infoHeader[INFO_HEADER_SIZE];
    
    // Image properties
    int width;
    int height;
    int bitsPerPixel;
    int rowSize;
    int dataSize;
    std::string path;
    
    // Performance monitoring
    mutable std::atomic<size_t> totalOperations{0};
    mutable std::atomic<size_t> parallelOperations{0};
    
    // Helper methods
    void readHeaders(std::ifstream& file);
    void writeHeaders(std::ofstream& file);
    int calculateRowSize(int width, int bpp);
    void validateImage();
    
    // Optimized parallel processing helpers
    void processChunkClockwise(const std::vector<unsigned char>& source, 
                              std::vector<unsigned char>& dest,
                              int startY, int endY, int oldWidth, int oldHeight,
                              int oldRowSize, int bytesPerPixel);
    
    void processChunkCounterClockwise(const std::vector<unsigned char>& source, 
                                    std::vector<unsigned char>& dest,
                                    int startY, int endY, int oldWidth, int oldHeight,
                                    int oldRowSize, int bytesPerPixel);
    
    void processChunkGaussian(const std::vector<unsigned char>& source, 
                             std::vector<unsigned char>& dest,
                             int startY, int endY, int bytesPerPixel);
    
    // Dynamic load balancing
    int calculateOptimalChunkSize(int totalWork, int numThreads) const;
    std::vector<std::pair<int, int>> createWorkChunks(int totalWork, int numThreads) const;
    
public:
    // Constructor
    BMPImageOptimized();
    
    // Destructor
    ~BMPImageOptimized() = default;
    
    // Copy constructor and assignment operator (deleted due to atomic members)
    BMPImageOptimized(const BMPImageOptimized& other) = delete;
    BMPImageOptimized& operator=(const BMPImageOptimized& other) = delete;
    
    // Move constructor and assignment operator (deleted due to atomic members)
    BMPImageOptimized(BMPImageOptimized&& other) noexcept = delete;
    BMPImageOptimized& operator=(BMPImageOptimized&& other) noexcept = delete;
    
    // Main interface methods
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename, const std::vector<unsigned char>& imageData);
    std::vector<unsigned char> getImageData() const;
    void setPath(const std::string& filepath);
    
    // Image processing methods
    void rotateClockwise(std::vector<unsigned char>& imageData);
    void rotateCounterClockwise(std::vector<unsigned char>& imageData);
    void applyGaussianFilter(std::vector<unsigned char>& imageData);
    
    // Advanced parallel processing methods
    void rotateClockwiseParallel(std::vector<unsigned char>& imageData, int numThreads = 0);
    void rotateCounterClockwiseParallel(std::vector<unsigned char>& imageData, int numThreads = 0);
    void applyGaussianFilterParallel(std::vector<unsigned char>& imageData, int numThreads = 0);
    
    // Pipeline processing (combines multiple operations)
    void processImagePipeline(const std::string& inputFile, int numThreads = 0);
    
    // Performance monitoring
    void resetPerformanceCounters();
    size_t getTotalOperations() const { return totalOperations.load(); }
    size_t getParallelOperations() const { return parallelOperations.load(); }
    double getParallelEfficiency() const;
    
    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getBitsPerPixel() const { return bitsPerPixel; }
    int getDataSize() const { return dataSize; }
    
    // Memory usage calculation
    size_t calculateMemoryUsage() const;
    
    // Benchmark methods
    struct BenchmarkResult {
        double sequentialTime;
        double parallelTime;
        double speedup;
        double efficiency;
        int numThreads;
    };
    
    BenchmarkResult benchmarkRotation(int numThreads = 0, int iterations = 5);
    BenchmarkResult benchmarkGaussianFilter(int numThreads = 0, int iterations = 5);
    std::vector<BenchmarkResult> benchmarkScaling(int maxThreads = 16);
};

#endif // WORKWITHBMP_OPTIMIZED_H
