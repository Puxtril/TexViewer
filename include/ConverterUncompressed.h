#pragma once

#include <cmath>

#include "BinaryReaderSlice.h"


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
};

// Convert variable-width source pixels to RGBA8888
// Currently only supports unsigned integers as source data
std::vector<uint8_t> convertUncompressedImage(const UncompressedLayout& layout, const char* rawData, size_t rawDataSize);
