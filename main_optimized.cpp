/*
   Optimized BMP Image Processing Main
   Author: Gleb Shikunov
   st128274@student.spbu.ru
   Lab1 - BMP Image Processing
*/

#include <iostream>
#include <chrono>
#include <memory>
#include <string>
#include <filesystem>
#include <iomanip>
#include <vector>
#include "WorkWithBMP_optimized.h"

std::string generateOutputFilename(const std::string& inputFile, const std::string& suffix) {
    std::filesystem::path inputPath(inputFile);
    std::string stem = inputPath.stem().string();
    std::string extension = inputPath.extension().string();
    return stem + "_" + suffix + extension;
}

void printMemoryUsage(const BMPImageOptimized& image) {
    size_t memoryUsage = image.calculateMemoryUsage();
    std::cout << "Memory usage: " << memoryUsage << " bytes (" 
              << memoryUsage / 1024.0 / 1024.0 << " MB)" << std::endl;
}

void printPerformanceStats(const BMPImageOptimized& image) {
    std::cout << "Performance Statistics:" << std::endl;
    std::cout << "  Total operations: " << image.getTotalOperations() << std::endl;
    std::cout << "  Parallel operations: " << image.getParallelOperations() << std::endl;
    std::cout << "  Parallel efficiency: " << std::fixed << std::setprecision(2) 
              << image.getParallelEfficiency() * 100.0 << "%" << std::endl;
}

void processImageOptimized(const std::string& inputFile, bool useParallel = false, int numThreads = 0) {
    try {
        std::cout << "=== Optimized BMP Image Processing ===" << std::endl;
        std::cout << "Input file: " << inputFile << std::endl;
        std::cout << "Parallel processing: " << (useParallel ? "Enabled" : "Disabled") << std::endl;
        if (useParallel) {
            std::cout << "Number of threads: " << (numThreads > 0 ? std::to_string(numThreads) : "Auto") << std::endl;
        }
        
        // Load image
        auto startTime = std::chrono::high_resolution_clock::now();
        BMPImageOptimized image;
        image.loadFromFile(inputFile);
        
        std::cout << "Image loaded successfully:" << std::endl;
        std::cout << "  Width: " << image.getWidth() << " pixels" << std::endl;
        std::cout << "  Height: " << image.getHeight() << " pixels" << std::endl;
        std::cout << "  Bits per pixel: " << image.getBitsPerPixel() << std::endl;
        std::cout << "  Data size: " << image.getDataSize() << " bytes" << std::endl;
        
        printMemoryUsage(image);
        
        // Get image data
        auto imageData = image.getImageData();
        std::cout << "Image data loaded: " << imageData.size() << " bytes" << std::endl;
        
        auto loadTime = std::chrono::high_resolution_clock::now();
        auto loadDuration = std::chrono::duration_cast<std::chrono::milliseconds>(loadTime - startTime);
        std::cout << "Load time: " << loadDuration.count() << " ms" << std::endl;
        
        // Process clockwise rotation
        std::cout << "\n--- Processing Clockwise Rotation ---" << std::endl;
        auto clockwiseData = imageData;
        auto rotateStart = std::chrono::high_resolution_clock::now();
        
        if (useParallel) {
            image.rotateClockwiseParallel(clockwiseData, numThreads);
        } else {
            image.rotateClockwise(clockwiseData);
        }
        
        auto rotateEnd = std::chrono::high_resolution_clock::now();
        auto rotateDuration = std::chrono::duration_cast<std::chrono::milliseconds>(rotateEnd - rotateStart);
        std::cout << "Clockwise rotation completed in: " << rotateDuration.count() << " ms" << std::endl;
        
        // Save clockwise rotated image
        std::string clockwiseFile = generateOutputFilename(inputFile, "rotated_clockwise_opt");
        image.saveToFile(clockwiseFile, clockwiseData);
        std::cout << "Saved clockwise rotated image: " << clockwiseFile << std::endl;
        
        // Apply Gaussian filter to clockwise rotated image
        std::cout << "\n--- Applying Gaussian Filter to Clockwise Rotated Image ---" << std::endl;
        auto filterStart = std::chrono::high_resolution_clock::now();
        
        if (useParallel) {
            image.applyGaussianFilterParallel(clockwiseData, numThreads);
        } else {
            image.applyGaussianFilter(clockwiseData);
        }
        
        auto filterEnd = std::chrono::high_resolution_clock::now();
        auto filterDuration = std::chrono::duration_cast<std::chrono::milliseconds>(filterEnd - filterStart);
        std::cout << "Gaussian filter applied in: " << filterDuration.count() << " ms" << std::endl;
        
        // Save filtered clockwise image
        std::string filteredClockwiseFile = generateOutputFilename(inputFile, "filtered_clockwise_opt");
        image.saveToFile(filteredClockwiseFile, clockwiseData);
        std::cout << "Saved filtered clockwise image: " << filteredClockwiseFile << std::endl;
        
        // Process counter-clockwise rotation
        std::cout << "\n--- Processing Counter-Clockwise Rotation ---" << std::endl;
        // Reload image for counter-clockwise rotation
        BMPImageOptimized image2;
        image2.loadFromFile(inputFile);
        auto counterClockwiseData = image2.getImageData();
        auto counterRotateStart = std::chrono::high_resolution_clock::now();
        
        if (useParallel) {
            image2.rotateCounterClockwiseParallel(counterClockwiseData, numThreads);
        } else {
            image2.rotateCounterClockwise(counterClockwiseData);
        }
        
        auto counterRotateEnd = std::chrono::high_resolution_clock::now();
        auto counterRotateDuration = std::chrono::duration_cast<std::chrono::milliseconds>(counterRotateEnd - counterRotateStart);
        std::cout << "Counter-clockwise rotation completed in: " << counterRotateDuration.count() << " ms" << std::endl;
        
        // Save counter-clockwise rotated image
        std::string counterClockwiseFile = generateOutputFilename(inputFile, "rotated_counter_clockwise_opt");
        image2.saveToFile(counterClockwiseFile, counterClockwiseData);
        std::cout << "Saved counter-clockwise rotated image: " << counterClockwiseFile << std::endl;
        
        // Apply Gaussian filter to counter-clockwise rotated image
        std::cout << "\n--- Applying Gaussian Filter to Counter-Clockwise Rotated Image ---" << std::endl;
        auto counterFilterStart = std::chrono::high_resolution_clock::now();
        
        if (useParallel) {
            image2.applyGaussianFilterParallel(counterClockwiseData, numThreads);
        } else {
            image2.applyGaussianFilter(counterClockwiseData);
        }
        
        auto counterFilterEnd = std::chrono::high_resolution_clock::now();
        auto counterFilterDuration = std::chrono::duration_cast<std::chrono::milliseconds>(counterFilterEnd - counterFilterStart);
        std::cout << "Gaussian filter applied in: " << counterFilterDuration.count() << " ms" << std::endl;
        
        // Save filtered counter-clockwise image
        std::string filteredCounterClockwiseFile = generateOutputFilename(inputFile, "filtered_counter_clockwise_opt");
        image2.saveToFile(filteredCounterClockwiseFile, counterClockwiseData);
        std::cout << "Saved filtered counter-clockwise image: " << filteredCounterClockwiseFile << std::endl;
        
        // Final timing summary
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "\n=== Processing Complete ===" << std::endl;
        std::cout << "Total processing time: " << totalDuration.count() << " ms" << std::endl;
        std::cout << "Memory efficiency: " << (image.calculateMemoryUsage() * 100.0 / image.getDataSize()) << "% of data size" << std::endl;
        
        // Print performance statistics
        printPerformanceStats(image);
        printPerformanceStats(image2);
        
        // Verify memory usage is within limits (200% of original data size)
        size_t maxAllowedMemory = image.getDataSize() * 2;
        if (image.calculateMemoryUsage() <= maxAllowedMemory) {
            std::cout << "✓ Memory usage is within acceptable limits" << std::endl;
        } else {
            std::cout << "⚠ Memory usage exceeds 200% of data size" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        throw;
    }
}

void runAdvancedBenchmark() {
    std::cout << "=== Advanced Performance Benchmark ===" << std::endl;
    
    try {
        BMPImageOptimized image;
        image.loadFromFile("example.bmp");
        
        std::cout << "Testing rotation performance scaling..." << std::endl;
        auto rotationResults = image.benchmarkScaling(8);
        
        std::cout << "\nTesting Gaussian filter performance scaling..." << std::endl;
        image.resetPerformanceCounters();
        auto filterResults = image.benchmarkScaling(8);
        
        std::cout << "\n=== Benchmark Summary ===" << std::endl;
        std::cout << "Best rotation speedup: " << std::fixed << std::setprecision(2) 
                  << std::max_element(rotationResults.begin(), rotationResults.end(),
                                     [](const auto& a, const auto& b) { return a.speedup < b.speedup; })->speedup
                  << "x" << std::endl;
        
        std::cout << "Best filter speedup: " << std::fixed << std::setprecision(2)
                  << std::max_element(filterResults.begin(), filterResults.end(),
                                     [](const auto& a, const auto& b) { return a.speedup < b.speedup; })->speedup
                  << "x" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Benchmark error: " << e.what() << std::endl;
    }
}

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [options] [input_file]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -p, --parallel     Enable parallel processing" << std::endl;
    std::cout << "  -t, --threads N    Number of threads (0 = auto)" << std::endl;
    std::cout << "  -b, --benchmark    Run performance benchmark" << std::endl;
    std::cout << "  -a, --advanced     Run advanced scaling benchmark" << std::endl;
    std::cout << "  -h, --help         Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  " << programName << " example.bmp" << std::endl;
    std::cout << "  " << programName << " -p -t 8 example.bmp" << std::endl;
    std::cout << "  " << programName << " --advanced" << std::endl;
}

int main(int argc, char* argv[]) {
    try {
        bool useParallel = false;
        int numThreads = 0;
        bool shouldRunBenchmark = false;
        bool shouldRunAdvancedBenchmark = false;
        std::string inputFile = "example.bmp";
        
        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "-h" || arg == "--help") {
                printUsage(argv[0]);
                return 0;
            } else if (arg == "-p" || arg == "--parallel") {
                useParallel = true;
            } else if (arg == "-t" || arg == "--threads") {
                if (i + 1 < argc) {
                    numThreads = std::stoi(argv[++i]);
                    useParallel = true;
                } else {
                    std::cerr << "Error: --threads requires a number" << std::endl;
                    return 1;
                }
            } else if (arg == "-b" || arg == "--benchmark") {
                shouldRunBenchmark = true;
            } else if (arg == "-a" || arg == "--advanced") {
                shouldRunAdvancedBenchmark = true;
            } else if (arg[0] != '-') {
                inputFile = arg;
            } else {
                std::cerr << "Unknown option: " << arg << std::endl;
                printUsage(argv[0]);
                return 1;
            }
        }
        
        if (shouldRunAdvancedBenchmark) {
            runAdvancedBenchmark();
        } else if (shouldRunBenchmark) {
            // Run basic benchmark
            processImageOptimized(inputFile, false, 0);
            processImageOptimized(inputFile, true, 4);
        } else {
            // Process the image
            processImageOptimized(inputFile, useParallel, numThreads);
        }
        
        std::cout << "\nAll operations completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
