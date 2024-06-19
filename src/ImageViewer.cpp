#include "ImageViewer.h"

ImageViewer::ImageViewer()
    : m_rawData(nullptr)
{
}

ImageViewer::~ImageViewer()
{
    if (m_rawData != nullptr)
        delete[] m_rawData;
}

void
ImageViewer::setWidget(QScrollArea* imagePreviewLayout)
{
    m_labelViewer = new QLabel();
    
    m_scrollArea = imagePreviewLayout;
    m_scrollArea->setBackgroundRole(QPalette::Dark);
    m_scrollArea->setWidget(m_labelViewer);
    m_scrollArea->setWidgetResizable(true);
}

void
ImageViewer::setIsCompressed(bool isCompressed)
{
    m_isCompressed = isCompressed;
    loadQImage();
}

void
ImageViewer::setImageData(char* data, size_t size)
{
    if (m_rawData != nullptr)
        delete[] m_rawData;

    m_rawData = data;
    m_rawDataSize = size;

    loadQImage();
}

void
ImageViewer::setWidth(int width)
{
    if ((width & (width - 1)) != 0)
    {
        emit errorMessage("Width is not power of 2");
        QPixmap pixmap;
        m_labelViewer->setPixmap(pixmap);
        return;
    }
    m_width = width;
    loadQImage();
}

void
ImageViewer::setFormat(ImageFormat format)
{
    m_format = format;
    loadQImage();
}

void
ImageViewer::setOffset(size_t dataOffset)
{
    m_rawDataOffset = dataOffset;
    loadQImage();
}

void
ImageViewer::setUncompressedStruct(UncompressedLayout layout)
{
    m_uncompressedLayout = layout;
    loadQImage();
}

void
ImageViewer::loadQImage()
{
    if (m_rawData == nullptr)
        return;

    try
    {
        if (m_isCompressed)
            loadQImageCompressed();
        else
            loadQImageUncompressed();
    }
    catch (std::exception& ex)
    {
        QPixmap pixmap;
        m_labelViewer->setPixmap(pixmap);
        emit errorMessage(ex.what());
    }
}

void
ImageViewer::loadQImageCompressed()
{
    const ImageData& data = g_imageFormatData.at(m_format);
    size_t rawDataSize = m_rawDataSize - m_rawDataOffset;
    char* rawData = m_rawData + m_rawDataOffset;

    int decompressedLen = getDecompressedSize(data.blockSizeSrc, data.pixelSizeDst, m_width, rawDataSize);
    std::vector<char> decompressed(decompressedLen);
    int height = convertBlocksSafe(rawData, rawDataSize, decompressed.data(), m_width, data.blockSizeSrc, data.pixelSizeDst, data.decode);
    QImage image((unsigned char*)decompressed.data(), m_width, height, data.qFormat);
    QPixmap pixmap = QPixmap::fromImage(image);
    m_labelViewer->setPixmap(pixmap);
}

void
ImageViewer::loadQImageUncompressed()
{
    std::vector<uint8_t> formattedData = convertUncompressedImage(m_uncompressedLayout, m_rawData + m_rawDataOffset, m_rawDataSize - m_rawDataOffset);
    size_t height = formattedData.size() / 4 / m_width;

    QImage image((unsigned char*)formattedData.data(), m_width, height, QImage::Format_RGBA8888);
    QPixmap pixmap = QPixmap::fromImage(image);
    m_labelViewer->setPixmap(pixmap);
}
