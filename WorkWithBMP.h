/*
   Gleb Shikunov
   st128274@student.spbu.ru
   Lab1 - BMP Image Processing
*/

#ifndef WORKWITHBMP_H
#define WORKWITHBMP_H

#include <string>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <memory>

class BMPImage {
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
    
    // Helper methods
    void readHeaders(std::ifstream& file);
    void writeHeaders(std::ofstream& file);
    int calculateRowSize(int width, int bpp);
    void validateImage();
    
public:
    // Constructor
    BMPImage();
    
    // Destructor
    ~BMPImage() = default;
    
    // Copy constructor and assignment operator
    BMPImage(const BMPImage& other) = default;
    BMPImage& operator=(const BMPImage& other) = default;
    
    // Move constructor and assignment operator
    BMPImage(BMPImage&& other) noexcept = default;
    BMPImage& operator=(BMPImage&& other) noexcept = default;
    
    // Main interface methods
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename, const std::vector<unsigned char>& imageData);
    std::vector<unsigned char> getImageData() const;
    void setPath(const std::string& filepath);
    
    // Image processing methods
    void rotateClockwise(std::vector<unsigned char>& imageData);
    void rotateCounterClockwise(std::vector<unsigned char>& imageData);
    void applyGaussianFilter(std::vector<unsigned char>& imageData);
    
    // Getters
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    int getBitsPerPixel() const { return bitsPerPixel; }
    int getDataSize() const { return dataSize; }
    
    // Memory usage calculation
    size_t calculateMemoryUsage() const;
};

#endif // WORKWITHBMP_H
