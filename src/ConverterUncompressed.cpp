#include "ConverterUncompressed.h"

std::vector<uint8_t>
convertUncompressedImage(const UncompressedLayout& layout, const char* rawData, size_t rawDataSize)
{
    size_t dstByteStride = 4;
    
    size_t srcBitStride = 0;
    for (const int& curBits : layout.bits)
    {
        if (curBits > 32)
            throw std::invalid_argument("Bit size cannot be >32");
        srcBitStride += curBits;
    }

    int srcMaxPixelCount = std::ceil((rawDataSize * 8) / srcBitStride);
    size_t dstSize = dstByteStride * srcMaxPixelCount;
    std::vector<uint8_t> formattedData(dstSize);

    BinaryReader::BinaryReaderSlice rawReader((uint8_t*)rawData, rawDataSize);

    // Pre-compute max
    std::vector<int> curBitMax(4);
    for (int x = 0; x < 4; x++)
        curBitMax[x] = std::pow(2, layout.bits[x]) - 1;

    for (int iPixel = 0; iPixel < srcMaxPixelCount; iPixel++)
    {
        for (int iChannel = 0; iChannel < layout.order.size(); iChannel++)
        {
            const int& curBits = layout.bits[iChannel];
            const int& curUnsignedMax = curBitMax[iChannel];
            const uint32_t read = rawReader.readBitwiseScalar<uint32_t>(curBits);
            const uint8_t readClamped = (read / (float)curUnsignedMax) * 255;

            formattedData[iPixel * 4 + 3] = 255;
            switch(layout.order[iChannel])
            {
            case(ColorChannel::RED):
                formattedData[iPixel * 4 + 0] = readClamped;
                break;
            case(ColorChannel::GREEN):
                formattedData[iPixel * 4 + 1] = readClamped;
                break;
            case(ColorChannel::BLUE):
                formattedData[iPixel * 4 + 2] = readClamped;
                break;
            case(ColorChannel::ALPHA):
                formattedData[iPixel * 4 + 3] = readClamped;
                break;
            case(ColorChannel::PADDING):
                continue;
            }
        }
    }

    return formattedData;
}