/*
   Gleb Shikunov
   st128274@student.spbu.ru
   Lab1 - BMP Image Processing
*/

#include <iostream>
#include <chrono>
#include <memory>
#include <string>
#include <filesystem>
#include "WorkWithBMP.h"

std::string generateOutputFilename(const std::string& inputFile, const std::string& suffix) {
    std::filesystem::path inputPath(inputFile);
    std::string stem = inputPath.stem().string();
    std::string extension = inputPath.extension().string();
    return stem + "_" + suffix + extension;
}

void printMemoryUsage(const BMPImage& image) {
    size_t memoryUsage = image.calculateMemoryUsage();
    std::cout << "Memory usage: " << memoryUsage << " bytes (" 
              << memoryUsage / 1024.0 / 1024.0 << " MB)" << std::endl;
}

void processImage(const std::string& inputFile) {
    try {
        std::cout << "=== BMP Image Processing ===" << std::endl;
        std::cout << "Input file: " << inputFile << std::endl;
        
        // Load image
        auto startTime = std::chrono::high_resolution_clock::now();
        BMPImage image;
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
        
        image.rotateClockwise(clockwiseData);
        
        auto rotateEnd = std::chrono::high_resolution_clock::now();
        auto rotateDuration = std::chrono::duration_cast<std::chrono::milliseconds>(rotateEnd - rotateStart);
        std::cout << "Clockwise rotation completed in: " << rotateDuration.count() << " ms" << std::endl;
        
        // Save clockwise rotated image
        std::string clockwiseFile = generateOutputFilename(inputFile, "rotated_clockwise");
        image.saveToFile(clockwiseFile, clockwiseData);
        std::cout << "Saved clockwise rotated image: " << clockwiseFile << std::endl;
        
        // Apply Gaussian filter to clockwise rotated image
        std::cout << "\n--- Applying Gaussian Filter to Clockwise Rotated Image ---" << std::endl;
        auto filterStart = std::chrono::high_resolution_clock::now();
        
        image.applyGaussianFilter(clockwiseData);
        
        auto filterEnd = std::chrono::high_resolution_clock::now();
        auto filterDuration = std::chrono::duration_cast<std::chrono::milliseconds>(filterEnd - filterStart);
        std::cout << "Gaussian filter applied in: " << filterDuration.count() << " ms" << std::endl;
        
        // Save filtered clockwise image
        std::string filteredClockwiseFile = generateOutputFilename(inputFile, "filtered_clockwise");
        image.saveToFile(filteredClockwiseFile, clockwiseData);
        std::cout << "Saved filtered clockwise image: " << filteredClockwiseFile << std::endl;
        
        // Process counter-clockwise rotation
        std::cout << "\n--- Processing Counter-Clockwise Rotation ---" << std::endl;
        // Reload image for counter-clockwise rotation
        BMPImage image2;
        image2.loadFromFile(inputFile);
        auto counterClockwiseData = image2.getImageData();
        auto counterRotateStart = std::chrono::high_resolution_clock::now();
        
        image2.rotateCounterClockwise(counterClockwiseData);
        
        auto counterRotateEnd = std::chrono::high_resolution_clock::now();
        auto counterRotateDuration = std::chrono::duration_cast<std::chrono::milliseconds>(counterRotateEnd - counterRotateStart);
        std::cout << "Counter-clockwise rotation completed in: " << counterRotateDuration.count() << " ms" << std::endl;
        
        // Save counter-clockwise rotated image
        std::string counterClockwiseFile = generateOutputFilename(inputFile, "rotated_counter_clockwise");
        image2.saveToFile(counterClockwiseFile, counterClockwiseData);
        std::cout << "Saved counter-clockwise rotated image: " << counterClockwiseFile << std::endl;
        
        // Apply Gaussian filter to counter-clockwise rotated image
        std::cout << "\n--- Applying Gaussian Filter to Counter-Clockwise Rotated Image ---" << std::endl;
        auto counterFilterStart = std::chrono::high_resolution_clock::now();
        
        image2.applyGaussianFilter(counterClockwiseData);
        
        auto counterFilterEnd = std::chrono::high_resolution_clock::now();
        auto counterFilterDuration = std::chrono::duration_cast<std::chrono::milliseconds>(counterFilterEnd - counterFilterStart);
        std::cout << "Gaussian filter applied in: " << counterFilterDuration.count() << " ms" << std::endl;
        
        // Save filtered counter-clockwise image
        std::string filteredCounterClockwiseFile = generateOutputFilename(inputFile, "filtered_counter_clockwise");
        image2.saveToFile(filteredCounterClockwiseFile, counterClockwiseData);
        std::cout << "Saved filtered counter-clockwise image: " << filteredCounterClockwiseFile << std::endl;
        
        // Final timing summary
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::cout << "\n=== Processing Complete ===" << std::endl;
        std::cout << "Total processing time: " << totalDuration.count() << " ms" << std::endl;
        std::cout << "Memory efficiency: " << (image.calculateMemoryUsage() * 100.0 / image.getDataSize()) << "% of data size" << std::endl;
        
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

int main() {
    try {
        // Process the sample image
        processImage("example.bmp");
        
        std::cout << "\nAll operations completed successfully!" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}
