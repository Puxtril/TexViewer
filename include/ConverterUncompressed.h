#pragma once

#include <cmath>

#include "BinaryReaderSlice.h"

enum UncompressedFormat
{
    UNORM,
    SNORM,
    FLOAT
};

enum ColorChannel
{
    RED,
    GREEN,
    BLUE,
    ALPHA,
    PADDING
};

struct UncompressedLayout
{
    // Both thse fields should be the same length
    std::vector<ColorChannel> order;
    std::vector<int> bits;
    UncompressedFormat format;
};

// For optimization during reading every pixel,
//  pre-calculate the max integer value given the integer bit width
std::vector<int> generateMaxUNorm(const std::vector<int>& bits);
std::vector<int> generateMaxSNorm(const std::vector<int>& bits);

// Read variable-width data from buffer and convert to uint8_t
typedef uint8_t (*readDataClamped)(BinaryReader::BinaryReaderSlice& reader, const int& bits, const int& maxInt);
uint8_t readUNorm(BinaryReader::BinaryReaderSlice& reader, const int& bits, const int& maxInt);
uint8_t readSNorm(BinaryReader::BinaryReaderSlice& reader, const int& bits, const int& maxInt);
uint8_t readFloat(BinaryReader::BinaryReaderSlice& reader, const int& bits, const int& maxInt);

// Convert variable-width source pixels to RGBA8888
std::vector<uint8_t> convertUncompressedImage(const UncompressedLayout& layout, const char* rawData, size_t rawDataSize);

// Not meant to be called directly
std::vector<uint8_t> _convertUncompressedImage(
    const UncompressedLayout& layout,
    const char* rawData,
    size_t rawDataSize,
    const std::vector<int>& maxNumForBitwidth,
    readDataClamped readDataFunc
);
