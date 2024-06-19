#pragma once

#include <QtGui/QImage>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qscrollarea.h>
#include <QtWidgets/qboxlayout.h>

#include "ConverterBlockCompressed.h"
#include "ConverterUncompressed.h"
#include "BinaryReaderSlice.h"

class ImageViewer : public QObject
{
    Q_OBJECT

    QLabel* m_labelViewer;
    QScrollArea* m_scrollArea;
    bool m_isCompressed;
    
    char* m_rawData;
    size_t m_rawDataSize;
    size_t m_rawDataOffset;
    int m_width;
    ImageFormat m_format;
    UncompressedLayout m_uncompressedLayout;
    QPixmap m_pixmap;

public:
    ImageViewer();
    ~ImageViewer();

    ImageViewer(const ImageViewer&) = delete;
    ImageViewer& operator=(const ImageViewer&) = delete;

    void setWidget(QScrollArea* imagePreviewLayout);

    void setIsCompressed(bool isCompressed);
    void setImageData(char* data, size_t size);
    void setWidth(int width);
    void setFormat(ImageFormat format);
    void setOffset(size_t dataOffset);
    void setUncompressedStruct(UncompressedLayout layout);

private:
    void loadQImage();
    void loadQImageCompressed();
    void loadQImageUncompressed();

signals:
    void errorMessage(const QString& msg, int timeout = 5000);
};