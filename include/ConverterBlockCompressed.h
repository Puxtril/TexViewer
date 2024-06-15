#pragma once

#include <QtGui/QImage>

#include <map>

#include "bcdec.h"

// Used in BCDEC to define block decode functions
typedef void(*decodeFunc)(const void*, void*, int);

// Specializations of bcdec's block decode functions
void bcdec_bc6hu(const void* compressedBlock, void* decompressedBlock, int destinationPitch);
void bcdec_bc6hs(const void* compressedBlock, void* decompressedBlock, int destinationPitch);
void _bcdec_bc6h_as_RGBA(const void* compressedBlock, void* decompressedBlock, int destinationPitch, bool isSigned);
void bcdec_bc5_as_RGB(const void* compressedBlock, void* decompressedBlock, int destinationPitch);

int getDecompressedSize(int srcBlockSize, int dstPixelSize, int width, int dataSize);
int convertBlocksSafe(const char* inData, size_t inDataLen, char* decompressed, int width, int srcBlockSize, int dstPixelSize, decodeFunc decodeFunc);

enum ImageFormat
{
    BC1,
    BC2,
    BC3,
    BC4,
    BC5,
    BC6HS,
    BC6HU,
    BC7
};

struct ImageData
{
    int blockSizeSrc;
    int pixelSizeDst;
    decodeFunc decode;
    QImage::Format qFormat;
};

static constexpr ImageData BC1Data   = {BCDEC_BC1_BLOCK_SIZE,  4,  &bcdec_bc1,        QImage::Format_RGBA8888  };
static constexpr ImageData BC2Data   = {BCDEC_BC2_BLOCK_SIZE,  4,  &bcdec_bc2,        QImage::Format_RGBA8888  };
static constexpr ImageData BC3Data   = {BCDEC_BC3_BLOCK_SIZE,  4,  &bcdec_bc3,        QImage::Format_RGBA8888  };
static constexpr ImageData BC4Data   = {BCDEC_BC4_BLOCK_SIZE,  1,  &bcdec_bc4,        QImage::Format_Grayscale8};
static constexpr ImageData BC5Data   = {BCDEC_BC5_BLOCK_SIZE,  3,  &bcdec_bc5_as_RGB, QImage::Format_RGB888    };
static constexpr ImageData BC6HUData = {BCDEC_BC6H_BLOCK_SIZE, 16, &bcdec_bc6hu,      QImage::Format_RGBX32FPx4};
static constexpr ImageData BC6HSData = {BCDEC_BC6H_BLOCK_SIZE, 16, &bcdec_bc6hs,      QImage::Format_RGBX32FPx4};
static constexpr ImageData BC7Data   = {BCDEC_BC7_BLOCK_SIZE,  4,  &bcdec_bc7,        QImage::Format_RGBA8888  };

static const std::map<ImageFormat, ImageData> g_imageFormatData = {
    {BC1, BC1Data},
    {BC2, BC2Data},
    {BC3, BC3Data},
    {BC4, BC4Data},
    {BC5, BC5Data},
    {BC6HU, BC6HUData},
    {BC6HS, BC6HSData},
    {BC7, BC7Data}
};