#include "ConverterBlockCompressed.h"

#define BCDEC_IMPLEMENTATION
#include "bcdec.h"

void
bcdec_bc6hu(const void* compressedBlock, void* decompressedBlock, int destinationPitch)
{
    _bcdec_bc6h_as_RGBA(compressedBlock, decompressedBlock, destinationPitch / 4, false);
}

void
bcdec_bc6hs(const void* compressedBlock, void* decompressedBlock, int destinationPitch)
{
    _bcdec_bc6h_as_RGBA(compressedBlock, decompressedBlock, destinationPitch / 4, true);
}

void
_bcdec_bc6h_as_RGBA(const void* compressedBlock, void* decompressedBlock, int destinationPitch, bool isSigned)
{
    unsigned short block[16*3];
    float* decompressed;
    const unsigned short* b;
    int i, j;

    bcdec_bc6h_half(compressedBlock, block, 4*3, isSigned);
    b = block;
    decompressed = (float*)decompressedBlock;
    for (i = 0; i < 4; ++i)
    {
        for (j = 0; j < 4; ++j)
        {
            // Previously: `j * 3`
            // Currently : `j * 4`
            decompressed[j * 4 + 0] = bcdec__half_to_float_quick(*b++);
            decompressed[j * 4 + 1] = bcdec__half_to_float_quick(*b++);
            decompressed[j * 4 + 2] = bcdec__half_to_float_quick(*b++);
        }
        decompressed += destinationPitch;
    }
}

void bcdec_bc5_as_RGB(const void* compressedBlock, void* decompressedBlock, int destinationPitch)
{
    bcdec__smooth_alpha_block(compressedBlock, decompressedBlock, destinationPitch, 3);
    bcdec__smooth_alpha_block(((char*)compressedBlock) + 8, ((char*)decompressedBlock) + 1, destinationPitch, 3);
}

int
getDecompressedSize(int srcBlockSize, int dstPixelSize, int width, int dataSize)
{
    // Dealing with 4x4 blocks here, not pixels
    int srcBlockRowByteSize = (width / 4) * srcBlockSize;
    int srcBlockHeightMax = std::ceil(dataSize / (float)srcBlockRowByteSize);
    int dstBlockByteSize = dstPixelSize * 16;
    int dstBlockRowSize = ceil(width / 4.0F) * dstBlockByteSize;

    return srcBlockHeightMax * dstBlockRowSize;
}

int
convertBlocksSafe(const char* inData, size_t inDataLen, char* decompressed, int width, int srcBlockSize, int dstPixelSize, decodeFunc decodeFunc)
{
    const char* src = inData;
    char* dst;

    int i = 0;
    while (true)
    {
        for (int j = 0; j < width; j+= 4)
        {
            if ((src + srcBlockSize - inData) >= inDataLen - 1)
                goto ret;
            dst = decompressed + (i * width + j) * dstPixelSize;
            decodeFunc(src, dst, width * dstPixelSize);
            src += srcBlockSize;
        }
        i += 4;
    }

    ret:
    return i;
}