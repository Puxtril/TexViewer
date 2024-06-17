#include "ConverterUncompressed.h"

std::vector<uint8_t>
convertUncompressedImage(const UncompressedLayout& layout, const char* rawData, size_t rawDataSize)
{
    switch(layout.format)
    {
        case(UncompressedFormat::UNORM):
            return _convertUncompressedImage(layout, rawData, rawDataSize, generateMaxUNorm(layout.bits), &readUNorm);
        case(UncompressedFormat::SNORM):
            return _convertUncompressedImage(layout, rawData, rawDataSize, generateMaxSNorm(layout.bits), &readSNorm);
        case(UncompressedFormat::FLOAT):
            return _convertUncompressedImage(layout, rawData, rawDataSize, {0, 0, 0, 0}, &readFloat);
    }
}

std::vector<uint8_t> _convertUncompressedImage(
    const UncompressedLayout& layout,
    const char* rawData,
    size_t rawDataSize,
    const std::vector<int>& maxNumForBitwidth,
    readDataClamped readDataFunc
)
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

    for (int iPixel = 0; iPixel < srcMaxPixelCount; iPixel++)
    {
        for (int iChannel = 0; iChannel < layout.order.size(); iChannel++)
        {
            const int& curBits = layout.bits[iChannel];
            const int& curMax = maxNumForBitwidth[iChannel];
            const uint8_t readClamped = readDataFunc(rawReader, curBits, curMax);

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

std::vector<int>
generateMaxUNorm(const std::vector<int>& bits)
{
    std::vector<int> curBitMax(4);
    for (int x = 0; x < 4; x++)
        curBitMax[x] = std::pow(2, bits[x]) - 1;
    return curBitMax;
}

std::vector<int>
generateMaxSNorm(const std::vector<int>& bits)
{
    std::vector<int> curBitMax(4);
    for (int x = 0; x < 4; x++)
        curBitMax[x] = std::pow(2, bits[x] - 1);
    return curBitMax;
}

uint8_t
readUNorm(BinaryReader::BinaryReaderSlice& reader, const int& bits, const int& maxInt)
{
    const uint32_t read = reader.readBitwiseScalar<uint32_t>(bits);
    return (read / (float)maxInt) * 255;
}

uint8_t
readSNorm(BinaryReader::BinaryReaderSlice& reader, const int& bits, const int& maxInt)
{
    const int32_t read = reader.readBitwiseScalar<int32_t>(bits);
    return ((read / (float)maxInt) + 1.0) * 127;
}

uint8_t
readFloat(BinaryReader::BinaryReaderSlice& reader, const int& bits, const int& maxInt)
{
    float read;
    if (bits == 16)
        read = reader.readHalf();
    else if (bits == 32)
        read = reader.readScalar<float>();
    else
        throw std::invalid_argument("Bit count is not 16 or 32");
    return std::max(0.0F, std::min(1.0F, read)) * 255.0F;
}
