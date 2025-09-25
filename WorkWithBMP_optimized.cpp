/*
   Optimized BMP Image Processing Implementation
   Author: Gleb Shikunov
   Student ID: st128274@student.spbu.ru
   Lab1 - BMP Image Processing
*/

#include "WorkWithBMP_optimized.h"
#include <iostream>
#include <cstring>
#include <numeric>

BMPImageOptimized::BMPImageOptimized() 
    : width(0), height(0), bitsPerPixel(0), rowSize(0), dataSize(0), path("") {
    // Initialize headers with zeros
    std::memset(fileHeader, 0, FILE_HEADER_SIZE);
    std::memset(infoHeader, 0, INFO_HEADER_SIZE);
}

void BMPImageOptimized::readHeaders(std::ifstream& file) {
    if (!file.is_open()) {
        throw std::runtime_error("File is not open");
    }
    
    // Read file header
    file.read(reinterpret_cast<char*>(fileHeader), FILE_HEADER_SIZE);
    if (file.gcount() != FILE_HEADER_SIZE) {
        throw std::runtime_error("Failed to read file header");
    }
    
    // Read info header
    file.read(reinterpret_cast<char*>(infoHeader), INFO_HEADER_SIZE);
    if (file.gcount() != INFO_HEADER_SIZE) {
        throw std::runtime_error("Failed to read info header");
    }
    
    // Extract image properties
    width = *reinterpret_cast<int*>(&infoHeader[4]);
    height = *reinterpret_cast<int*>(&infoHeader[8]);
    bitsPerPixel = *reinterpret_cast<short*>(&infoHeader[14]);
    
    // Calculate derived properties
    rowSize = calculateRowSize(width, bitsPerPixel);
    dataSize = rowSize * height;
    
    // Validate image properties
    validateImage();
}

void BMPImageOptimized::writeHeaders(std::ofstream& file) {
    if (!file.is_open()) {
        throw std::runtime_error("File is not open for writing");
    }
    
    // Write file header
    file.write(reinterpret_cast<const char*>(fileHeader), FILE_HEADER_SIZE);
    
    // Write info header
    file.write(reinterpret_cast<const char*>(infoHeader), INFO_HEADER_SIZE);
}

int BMPImageOptimized::calculateRowSize(int width, int bpp) {
    // Calculate row size with padding (must be multiple of 4)
    int bytesPerPixel = bpp / 8;
    int rowSize = width * bytesPerPixel;
    return (rowSize + 3) & ~3; // Round up to nearest multiple of 4
}

void BMPImageOptimized::validateImage() {
    if (width <= 0 || height <= 0) {
        throw std::runtime_error("Invalid image dimensions");
    }
    
    if (bitsPerPixel != 24 && bitsPerPixel != 32) {
        throw std::runtime_error("Unsupported bits per pixel: " + std::to_string(bitsPerPixel));
    }
    
    if (dataSize <= 0) {
        throw std::runtime_error("Invalid data size");
    }
}

void BMPImageOptimized::loadFromFile(const std::string& filename) {
    path = filename;
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    try {
        readHeaders(file);
    } catch (const std::exception& e) {
        file.close();
        throw;
    }
    
    file.close();
}

void BMPImageOptimized::setPath(const std::string& filepath) {
    path = filepath;
}

void BMPImageOptimized::saveToFile(const std::string& filename, const std::vector<unsigned char>& imageData) {
    if (imageData.size() != static_cast<size_t>(dataSize)) {
        throw std::runtime_error("Image data size mismatch");
    }
    
    // Create new headers for the current dimensions
    unsigned char newFileHeader[FILE_HEADER_SIZE];
    unsigned char newInfoHeader[INFO_HEADER_SIZE];
    
    // Copy original headers
    std::memcpy(newFileHeader, fileHeader, FILE_HEADER_SIZE);
    std::memcpy(newInfoHeader, infoHeader, INFO_HEADER_SIZE);
    
    // Calculate palette size
    int dataOffset = *reinterpret_cast<const int*>(&fileHeader[10]);
    int paletteSize = dataOffset - FILE_HEADER_SIZE - INFO_HEADER_SIZE;
    
    // Update file size in header (including palette)
    int fileSize = FILE_HEADER_SIZE + INFO_HEADER_SIZE + paletteSize + dataSize;
    newFileHeader[2] = fileSize & 0xFF;
    newFileHeader[3] = (fileSize >> 8) & 0xFF;
    newFileHeader[4] = (fileSize >> 16) & 0xFF;
    newFileHeader[5] = (fileSize >> 24) & 0xFF;
    
    // Update data offset
    int newDataOffset = FILE_HEADER_SIZE + INFO_HEADER_SIZE + paletteSize;
    *reinterpret_cast<int*>(&newFileHeader[10]) = newDataOffset;
    
    // Update width and height in info header
    *reinterpret_cast<int*>(&newInfoHeader[4]) = width;
    *reinterpret_cast<int*>(&newInfoHeader[8]) = height;
    
    // Update image size in info header
    *reinterpret_cast<int*>(&newInfoHeader[20]) = dataSize;
    
    // Update compression (0 = no compression)
    *reinterpret_cast<int*>(&newInfoHeader[16]) = 0;
    
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot create file: " + filename);
    }
    
    try {
        // Write updated headers
        file.write(reinterpret_cast<const char*>(newFileHeader), FILE_HEADER_SIZE);
        file.write(reinterpret_cast<const char*>(newInfoHeader), INFO_HEADER_SIZE);
        
        // Write palette if it exists
        if (paletteSize > 0) {
            std::ifstream originalFile(path, std::ios::binary);
            if (originalFile.is_open()) {
                originalFile.seekg(FILE_HEADER_SIZE + INFO_HEADER_SIZE);
                std::vector<unsigned char> palette(paletteSize);
                originalFile.read(reinterpret_cast<char*>(palette.data()), paletteSize);
                file.write(reinterpret_cast<const char*>(palette.data()), paletteSize);
                originalFile.close();
            }
        }
        
        // Write image data
        file.write(reinterpret_cast<const char*>(imageData.data()), dataSize);
    } catch (const std::exception& e) {
        file.close();
        throw;
    }
    
    file.close();
}

std::vector<unsigned char> BMPImageOptimized::getImageData() const {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for reading data");
    }
    
    // Get data offset from file header
    int dataOffset = *reinterpret_cast<const int*>(&fileHeader[10]);
    
    // Skip to data offset
    file.seekg(dataOffset);
    
    std::vector<unsigned char> data(dataSize);
    file.read(reinterpret_cast<char*>(data.data()), dataSize);
    
    if (file.gcount() != dataSize) {
        throw std::runtime_error("Failed to read complete image data");
    }
    
    file.close();
    return data;
}

// Sequential implementations (same as original)
void BMPImageOptimized::rotateClockwise(std::vector<unsigned char>& imageData) {
    if (imageData.size() != static_cast<size_t>(dataSize)) {
        throw std::runtime_error("Image data size mismatch for rotation");
    }
    
    int bytesPerPixel = bitsPerPixel / 8;
    int oldWidth = width;
    int oldHeight = height;
    int oldRowSize = rowSize;
    
    // Update dimensions first
    std::swap(width, height);
    rowSize = calculateRowSize(width, bitsPerPixel);
    dataSize = rowSize * height;
    
    // Create new data with proper padding
    std::vector<unsigned char> newData(dataSize, 0);
    
    for (int y = 0; y < oldHeight; ++y) {
        for (int x = 0; x < oldWidth; ++x) {
            for (int c = 0; c < bytesPerPixel; ++c) {
                // Clockwise rotation: (x, y) -> (y, oldWidth - 1 - x)
                int newX = y;
                int newY = oldWidth - 1 - x;
                if (newX < width && newY < height) {
                    newData[newY * rowSize + newX * bytesPerPixel + c] = 
                        imageData[y * oldRowSize + x * bytesPerPixel + c];
                }
            }
        }
    }
    
    imageData = std::move(newData);
    totalOperations.fetch_add(1);
}

void BMPImageOptimized::rotateCounterClockwise(std::vector<unsigned char>& imageData) {
    if (imageData.size() != static_cast<size_t>(dataSize)) {
        throw std::runtime_error("Image data size mismatch for rotation");
    }
    
    int bytesPerPixel = bitsPerPixel / 8;
    int oldWidth = width;
    int oldHeight = height;
    int oldRowSize = rowSize;
    
    // Update dimensions first
    std::swap(width, height);
    rowSize = calculateRowSize(width, bitsPerPixel);
    dataSize = rowSize * height;
    
    // Create new data with proper padding
    std::vector<unsigned char> newData(dataSize, 0);
    
    for (int y = 0; y < oldHeight; ++y) {
        for (int x = 0; x < oldWidth; ++x) {
            for (int c = 0; c < bytesPerPixel; ++c) {
                // Counter-clockwise rotation: (x, y) -> (oldHeight - 1 - y, x)
                int newX = oldHeight - 1 - y;
                int newY = x;
                if (newX < width && newY < height) {
                    newData[newY * rowSize + newX * bytesPerPixel + c] = 
                        imageData[y * oldRowSize + x * bytesPerPixel + c];
                }
            }
        }
    }
    
    imageData = std::move(newData);
    totalOperations.fetch_add(1);
}

void BMPImageOptimized::applyGaussianFilter(std::vector<unsigned char>& imageData) {
    if (imageData.size() != static_cast<size_t>(dataSize)) {
        throw std::runtime_error("Image data size mismatch for filtering");
    }
    
    // Gaussian kernel 3x3
    const float kernel[3][3] = {
        {1.0f/16, 2.0f/16, 1.0f/16},
        {2.0f/16, 4.0f/16, 2.0f/16},
        {1.0f/16, 2.0f/16, 1.0f/16}
    };
    
    int bytesPerPixel = bitsPerPixel / 8;
    std::vector<unsigned char> filtered(dataSize);
    
    // Copy original data to filtered
    std::copy(imageData.begin(), imageData.end(), filtered.begin());
    
    // Apply filter to inner pixels only
    for (int y = 1; y < height - 1; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            for (int c = 0; c < bytesPerPixel; ++c) {
                float sum = 0.0f;
                
                for (int ky = 0; ky < 3; ++ky) {
                    for (int kx = 0; kx < 3; ++kx) {
                        int px = x + kx - 1;
                        int py = y + ky - 1;
                        sum += kernel[ky][kx] * imageData[py * rowSize + px * bytesPerPixel + c];
                    }
                }
                
                // Clamp to valid range
                filtered[y * rowSize + x * bytesPerPixel + c] = 
                    static_cast<unsigned char>(std::max(0.0f, std::min(255.0f, sum)));
            }
        }
    }
    
    imageData = std::move(filtered);
    totalOperations.fetch_add(1);
}

// Optimized parallel implementations

int BMPImageOptimized::calculateOptimalChunkSize(int totalWork, int numThreads) const {
    // Calculate optimal chunk size based on work size and number of threads
    int minChunkSize = 1;
    int maxChunkSize = totalWork / numThreads;
    
    // Ensure chunk size is not too small (avoid overhead) or too large (poor load balancing)
    int optimalChunkSize = std::max(minChunkSize, std::min(maxChunkSize, totalWork / (numThreads * 4)));
    
    return optimalChunkSize;
}

std::vector<std::pair<int, int>> BMPImageOptimized::createWorkChunks(int totalWork, int numThreads) const {
    std::vector<std::pair<int, int>> chunks;
    int chunkSize = calculateOptimalChunkSize(totalWork, numThreads);
    
    for (int i = 0; i < totalWork; i += chunkSize) {
        int end = std::min(i + chunkSize, totalWork);
        chunks.emplace_back(i, end);
    }
    
    return chunks;
}

void BMPImageOptimized::processChunkClockwise(const std::vector<unsigned char>& source, 
                                             std::vector<unsigned char>& dest,
                                             int startY, int endY, int oldWidth, int /* oldHeight */,
                                             int oldRowSize, int bytesPerPixel) {
    for (int y = startY; y < endY; ++y) {
        for (int x = 0; x < oldWidth; ++x) {
            for (int c = 0; c < bytesPerPixel; ++c) {
                int newX = y;
                int newY = oldWidth - 1 - x;
                if (newX < width && newY < height) {
                    dest[newY * rowSize + newX * bytesPerPixel + c] = 
                        source[y * oldRowSize + x * bytesPerPixel + c];
                }
            }
        }
    }
}

void BMPImageOptimized::processChunkCounterClockwise(const std::vector<unsigned char>& source, 
                                                    std::vector<unsigned char>& dest,
                                                    int startY, int endY, int oldWidth, int oldHeight,
                                                    int oldRowSize, int bytesPerPixel) {
    for (int y = startY; y < endY; ++y) {
        for (int x = 0; x < oldWidth; ++x) {
            for (int c = 0; c < bytesPerPixel; ++c) {
                int newX = oldHeight - 1 - y;
                int newY = x;
                if (newX < width && newY < height) {
                    dest[newY * rowSize + newX * bytesPerPixel + c] = 
                        source[y * oldRowSize + x * bytesPerPixel + c];
                }
            }
        }
    }
}

void BMPImageOptimized::processChunkGaussian(const std::vector<unsigned char>& source, 
                                            std::vector<unsigned char>& dest,
                                            int startY, int endY, int bytesPerPixel) {
    const float kernel[3][3] = {
        {1.0f/16, 2.0f/16, 1.0f/16},
        {2.0f/16, 4.0f/16, 2.0f/16},
        {1.0f/16, 2.0f/16, 1.0f/16}
    };
    
    for (int y = startY; y < endY; ++y) {
        for (int x = 1; x < width - 1; ++x) {
            for (int c = 0; c < bytesPerPixel; ++c) {
                float sum = 0.0f;
                
                for (int ky = 0; ky < 3; ++ky) {
                    for (int kx = 0; kx < 3; ++kx) {
                        int px = x + kx - 1;
                        int py = y + ky - 1;
                        sum += kernel[ky][kx] * source[py * rowSize + px * bytesPerPixel + c];
                    }
                }
                
                dest[y * rowSize + x * bytesPerPixel + c] = 
                    static_cast<unsigned char>(std::max(0.0f, std::min(255.0f, sum)));
            }
        }
    }
}

void BMPImageOptimized::rotateClockwiseParallel(std::vector<unsigned char>& imageData, int numThreads) {
    if (imageData.size() != static_cast<size_t>(dataSize)) {
        throw std::runtime_error("Image data size mismatch for rotation");
    }
    
    int bytesPerPixel = bitsPerPixel / 8;
    int oldWidth = width;
    int oldHeight = height;
    int oldRowSize = rowSize;
    
    // Update dimensions first
    std::swap(width, height);
    rowSize = calculateRowSize(width, bitsPerPixel);
    dataSize = rowSize * height;
    
    // Create new data with proper padding
    std::vector<unsigned char> newData(dataSize, 0);
    
    // Determine number of threads
    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4; // fallback
    }
    
    // Limit threads based on work size
    numThreads = std::min(numThreads, oldHeight);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
#ifdef _OPENMP
    #pragma omp parallel for num_threads(numThreads) schedule(dynamic)
    for (int y = 0; y < oldHeight; ++y) {
        processChunkClockwise(imageData, newData, y, y + 1, oldWidth, oldHeight, oldRowSize, bytesPerPixel);
    }
#else
    // Use std::thread with dynamic load balancing
    std::vector<std::thread> threads;
    auto chunks = createWorkChunks(oldHeight, numThreads);
    
    for (const auto& chunk : chunks) {
        threads.emplace_back([&, chunk]() {
            processChunkClockwise(imageData, newData, chunk.first, chunk.second, 
                                oldWidth, oldHeight, oldRowSize, bytesPerPixel);
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
#endif
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    imageData = std::move(newData);
    totalOperations.fetch_add(1);
    parallelOperations.fetch_add(1);
    
    std::cout << "Parallel clockwise rotation completed in " << duration.count() << " μs with " 
              << numThreads << " threads" << std::endl;
}

void BMPImageOptimized::rotateCounterClockwiseParallel(std::vector<unsigned char>& imageData, int numThreads) {
    if (imageData.size() != static_cast<size_t>(dataSize)) {
        throw std::runtime_error("Image data size mismatch for rotation");
    }
    
    int bytesPerPixel = bitsPerPixel / 8;
    int oldWidth = width;
    int oldHeight = height;
    int oldRowSize = rowSize;
    
    // Update dimensions first
    std::swap(width, height);
    rowSize = calculateRowSize(width, bitsPerPixel);
    dataSize = rowSize * height;
    
    // Create new data with proper padding
    std::vector<unsigned char> newData(dataSize, 0);
    
    // Determine number of threads
    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4; // fallback
    }
    
    // Limit threads based on work size
    numThreads = std::min(numThreads, oldHeight);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
#ifdef _OPENMP
    #pragma omp parallel for num_threads(numThreads) schedule(dynamic)
    for (int y = 0; y < oldHeight; ++y) {
        processChunkCounterClockwise(imageData, newData, y, y + 1, oldWidth, oldHeight, oldRowSize, bytesPerPixel);
    }
#else
    // Use std::thread with dynamic load balancing
    std::vector<std::thread> threads;
    auto chunks = createWorkChunks(oldHeight, numThreads);
    
    for (const auto& chunk : chunks) {
        threads.emplace_back([&, chunk]() {
            processChunkCounterClockwise(imageData, newData, chunk.first, chunk.second, 
                                       oldWidth, oldHeight, oldRowSize, bytesPerPixel);
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
#endif
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    imageData = std::move(newData);
    totalOperations.fetch_add(1);
    parallelOperations.fetch_add(1);
    
    std::cout << "Parallel counter-clockwise rotation completed in " << duration.count() << " μs with " 
              << numThreads << " threads" << std::endl;
}

void BMPImageOptimized::applyGaussianFilterParallel(std::vector<unsigned char>& imageData, int numThreads) {
    if (imageData.size() != static_cast<size_t>(dataSize)) {
        throw std::runtime_error("Image data size mismatch for filtering");
    }
    
    int bytesPerPixel = bitsPerPixel / 8;
    std::vector<unsigned char> filtered(dataSize);
    
    // Copy original data to filtered
    std::copy(imageData.begin(), imageData.end(), filtered.begin());
    
    // Determine number of threads
    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4; // fallback
    }
    
    // Limit threads based on work size
    int workHeight = height - 2; // Exclude borders
    numThreads = std::min(numThreads, workHeight);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
#ifdef _OPENMP
    #pragma omp parallel for num_threads(numThreads) schedule(dynamic)
    for (int y = 1; y < height - 1; ++y) {
        processChunkGaussian(imageData, filtered, y, y + 1, bytesPerPixel);
    }
#else
    // Use std::thread with dynamic load balancing
    std::vector<std::thread> threads;
    auto chunks = createWorkChunks(workHeight, numThreads);
    
    for (const auto& chunk : chunks) {
        threads.emplace_back([&, chunk]() {
            processChunkGaussian(imageData, filtered, chunk.first + 1, chunk.second + 1, bytesPerPixel);
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
#endif
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    
    imageData = std::move(filtered);
    totalOperations.fetch_add(1);
    parallelOperations.fetch_add(1);
    
    std::cout << "Parallel Gaussian filter completed in " << duration.count() << " μs with " 
              << numThreads << " threads" << std::endl;
}

// Performance monitoring methods
void BMPImageOptimized::resetPerformanceCounters() {
    totalOperations.store(0);
    parallelOperations.store(0);
}

double BMPImageOptimized::getParallelEfficiency() const {
    size_t total = totalOperations.load();
    size_t parallel = parallelOperations.load();
    
    if (total == 0) return 0.0;
    return static_cast<double>(parallel) / total;
}

size_t BMPImageOptimized::calculateMemoryUsage() const {
    return sizeof(*this) + dataSize;
}

// Benchmark methods
BMPImageOptimized::BenchmarkResult BMPImageOptimized::benchmarkRotation(int numThreads, int iterations) {
    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4;
    }
    
    BenchmarkResult result;
    result.numThreads = numThreads;
    
    // Sequential benchmark
    double totalSequential = 0.0;
    for (int i = 0; i < iterations; ++i) {
        // Create a fresh copy for each iteration
        BMPImageOptimized tempImage;
        tempImage.loadFromFile(path);
        auto data = tempImage.getImageData();
        auto start = std::chrono::high_resolution_clock::now();
        tempImage.rotateClockwise(data);
        auto end = std::chrono::high_resolution_clock::now();
        totalSequential += std::chrono::duration<double, std::milli>(end - start).count();
    }
    result.sequentialTime = totalSequential / iterations;
    
    // Parallel benchmark
    double totalParallel = 0.0;
    for (int i = 0; i < iterations; ++i) {
        // Create a fresh copy for each iteration
        BMPImageOptimized tempImage;
        tempImage.loadFromFile(path);
        auto data = tempImage.getImageData();
        auto start = std::chrono::high_resolution_clock::now();
        tempImage.rotateClockwiseParallel(data, numThreads);
        auto end = std::chrono::high_resolution_clock::now();
        totalParallel += std::chrono::duration<double, std::milli>(end - start).count();
    }
    result.parallelTime = totalParallel / iterations;
    
    result.speedup = result.sequentialTime / result.parallelTime;
    result.efficiency = result.speedup / numThreads;
    
    return result;
}

BMPImageOptimized::BenchmarkResult BMPImageOptimized::benchmarkGaussianFilter(int numThreads, int iterations) {
    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4;
    }
    
    BenchmarkResult result;
    result.numThreads = numThreads;
    
    // Sequential benchmark
    double totalSequential = 0.0;
    for (int i = 0; i < iterations; ++i) {
        // Create a fresh copy for each iteration
        BMPImageOptimized tempImage;
        tempImage.loadFromFile(path);
        auto data = tempImage.getImageData();
        auto start = std::chrono::high_resolution_clock::now();
        tempImage.applyGaussianFilter(data);
        auto end = std::chrono::high_resolution_clock::now();
        totalSequential += std::chrono::duration<double, std::milli>(end - start).count();
    }
    result.sequentialTime = totalSequential / iterations;
    
    // Parallel benchmark
    double totalParallel = 0.0;
    for (int i = 0; i < iterations; ++i) {
        // Create a fresh copy for each iteration
        BMPImageOptimized tempImage;
        tempImage.loadFromFile(path);
        auto data = tempImage.getImageData();
        auto start = std::chrono::high_resolution_clock::now();
        tempImage.applyGaussianFilterParallel(data, numThreads);
        auto end = std::chrono::high_resolution_clock::now();
        totalParallel += std::chrono::duration<double, std::milli>(end - start).count();
    }
    result.parallelTime = totalParallel / iterations;
    
    result.speedup = result.sequentialTime / result.parallelTime;
    result.efficiency = result.speedup / numThreads;
    
    return result;
}

std::vector<BMPImageOptimized::BenchmarkResult> BMPImageOptimized::benchmarkScaling(int maxThreads) {
    std::vector<BenchmarkResult> results;
    
    for (int threads = 1; threads <= maxThreads; ++threads) {
        auto rotationResult = benchmarkRotation(threads, 3);
        auto filterResult = benchmarkGaussianFilter(threads, 3);
        
        std::cout << "Threads: " << threads 
                  << " | Rotation speedup: " << std::fixed << std::setprecision(2) << rotationResult.speedup
                  << " | Filter speedup: " << filterResult.speedup << std::endl;
        
        results.push_back(rotationResult);
    }
    
    return results;
}
