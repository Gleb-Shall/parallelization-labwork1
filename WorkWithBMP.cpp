/*
   Gleb Shikunov
   st128274@student.spbu.ru
   Lab1 - BMP Image Processing
*/

#include "WorkWithBMP.h"
#include <iostream>
#include <cstring>

BMPImage::BMPImage() 
    : width(0), height(0), bitsPerPixel(0), rowSize(0), dataSize(0), path("") {
    // Initialize headers with zeros
    std::memset(fileHeader, 0, FILE_HEADER_SIZE);
    std::memset(infoHeader, 0, INFO_HEADER_SIZE);
}

void BMPImage::readHeaders(std::ifstream& file) {
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

void BMPImage::writeHeaders(std::ofstream& file) {
    if (!file.is_open()) {
        throw std::runtime_error("File is not open for writing");
    }
    
    // Write file header
    file.write(reinterpret_cast<const char*>(fileHeader), FILE_HEADER_SIZE);
    
    // Write info header
    file.write(reinterpret_cast<const char*>(infoHeader), INFO_HEADER_SIZE);
}

int BMPImage::calculateRowSize(int width, int bpp) {
    // Calculate row size with padding (must be multiple of 4)
    int bytesPerPixel = bpp / 8;
    int rowSize = width * bytesPerPixel;
    return (rowSize + 3) & ~3; // Round up to nearest multiple of 4
}

void BMPImage::validateImage() {
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

void BMPImage::loadFromFile(const std::string& filename) {
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

void BMPImage::setPath(const std::string& filepath) {
    path = filepath;
}

void BMPImage::saveToFile(const std::string& filename, const std::vector<unsigned char>& imageData) {
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

std::vector<unsigned char> BMPImage::getImageData() const {
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

void BMPImage::rotateClockwise(std::vector<unsigned char>& imageData) {
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
}

void BMPImage::rotateCounterClockwise(std::vector<unsigned char>& imageData) {
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
}

void BMPImage::applyGaussianFilter(std::vector<unsigned char>& imageData) {
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
}

size_t BMPImage::calculateMemoryUsage() const {
    return sizeof(*this) + dataSize;
}

// Parallel processing implementations

void BMPImage::rotateClockwiseParallel(std::vector<unsigned char>& imageData, int numThreads) {
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
    
#ifdef _OPENMP
    #pragma omp parallel for num_threads(numThreads) collapse(2)
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
#else
    // Fallback to std::thread if OpenMP is not available
    std::vector<std::thread> threads;
    int rowsPerThread = oldHeight / numThreads;
    
    for (int t = 0; t < numThreads; ++t) {
        int startY = t * rowsPerThread;
        int endY = (t == numThreads - 1) ? oldHeight : (t + 1) * rowsPerThread;
        
        threads.emplace_back([&, startY, endY, oldWidth, oldRowSize, bytesPerPixel]() {
            for (int y = startY; y < endY; ++y) {
                for (int x = 0; x < oldWidth; ++x) {
                    for (int c = 0; c < bytesPerPixel; ++c) {
                        int newX = y;
                        int newY = oldWidth - 1 - x;
                        if (newX < width && newY < height) {
                            newData[newY * rowSize + newX * bytesPerPixel + c] = 
                                imageData[y * oldRowSize + x * bytesPerPixel + c];
                        }
                    }
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
#endif
    
    imageData = std::move(newData);
}

void BMPImage::rotateCounterClockwiseParallel(std::vector<unsigned char>& imageData, int numThreads) {
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
    
#ifdef _OPENMP
    #pragma omp parallel for num_threads(numThreads) collapse(2)
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
#else
    // Fallback to std::thread if OpenMP is not available
    std::vector<std::thread> threads;
    int rowsPerThread = oldHeight / numThreads;
    
    for (int t = 0; t < numThreads; ++t) {
        int startY = t * rowsPerThread;
        int endY = (t == numThreads - 1) ? oldHeight : (t + 1) * rowsPerThread;
        
        threads.emplace_back([&, startY, endY, oldWidth, oldRowSize, bytesPerPixel]() {
            for (int y = startY; y < endY; ++y) {
                for (int x = 0; x < oldWidth; ++x) {
                    for (int c = 0; c < bytesPerPixel; ++c) {
                        int newX = oldHeight - 1 - y;
                        int newY = x;
                        if (newX < width && newY < height) {
                            newData[newY * rowSize + newX * bytesPerPixel + c] = 
                                imageData[y * oldRowSize + x * bytesPerPixel + c];
                        }
                    }
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
#endif
    
    imageData = std::move(newData);
}

void BMPImage::applyGaussianFilterParallel(std::vector<unsigned char>& imageData, int numThreads) {
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
    
    // Determine number of threads
    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4; // fallback
    }
    
#ifdef _OPENMP
    #pragma omp parallel for num_threads(numThreads) collapse(2)
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
#else
    // Fallback to std::thread if OpenMP is not available
    std::vector<std::thread> threads;
    int rowsPerThread = (height - 2) / numThreads;
    
    for (int t = 0; t < numThreads; ++t) {
        int startY = 1 + t * rowsPerThread;
        int endY = (t == numThreads - 1) ? height - 1 : 1 + (t + 1) * rowsPerThread;
        
        threads.emplace_back([&, startY, endY, bytesPerPixel]() {
            for (int y = startY; y < endY; ++y) {
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
                        
                        filtered[y * rowSize + x * bytesPerPixel + c] = 
                            static_cast<unsigned char>(std::max(0.0f, std::min(255.0f, sum)));
                    }
                }
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
#endif
    
    imageData = std::move(filtered);
}

void BMPImage::processImageParallel(const std::string& inputFile, int numThreads) {
    // This method can be used for complete parallel processing pipeline
    // Implementation would combine all parallel operations
    // For now, it's a placeholder for future enhancement
    (void)inputFile; // Suppress unused parameter warning
    if (numThreads <= 0) {
        numThreads = std::thread::hardware_concurrency();
        if (numThreads == 0) numThreads = 4;
    }
    
    std::cout << "Processing image with " << numThreads << " threads" << std::endl;
    // Additional parallel processing logic can be added here
}
