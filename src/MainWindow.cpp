#include "MainWindow.h"

MainWindow::MainWindow()
{
    m_dialog.setAcceptMode(QFileDialog::AcceptOpen);
    m_dialog.setFileMode(QFileDialog::ExistingFile);
    m_dialog.setOption(QFileDialog::ReadOnly, true);
    m_dialog.setOption(QFileDialog::HideNameFilterDetails, true);
    m_dialog.setModal(true);   
}

void
MainWindow::setupUi(QMainWindow* mainWindow)
{
    m_mainWindow = mainWindow;
    Ui_MainWindow::setupUi(mainWindow);
    m_viewer.setWidget(this->ImagePreviewArea);

    mainWindow->setWindowTitle(mainWindow->windowTitle() + " " + TEXVIEWER_VERSION_STR);

    Pow2Spinbox* newWidthSpinbox = new Pow2Spinbox();
    this->WidthInput->hide();
    this->XResolutionLayout->replaceWidget(this->WidthInput, newWidthSpinbox);
    this->WidthInput = newWidthSpinbox;

    Pow2Spinbox* newHeightSpinbox = new Pow2Spinbox();
    this->HeightInput->hide();
    this->YResolutionLayout->replaceWidget(this->HeightInput, newHeightSpinbox);
    this->HeightInput = newHeightSpinbox;
    this->HeightInput->setSpecialValueText("Auto");

    connect(this->FormatOptionsGroup, &QTabWidget::currentChanged, this, &MainWindow::formatTabChanged);
    connect(this->actionOpen, &QAction::triggered, this, &MainWindow::fileOpenTriggered);
    connect(&m_dialog, &QFileDialog::fileSelected, this, &MainWindow::fileSelected);
    connect(this->FormatCombo, &QComboBox::currentIndexChanged, this, &MainWindow::compressedFormatComboChanged);
    connect(this->OffsetCombo, &QComboBox::currentIndexChanged, this, &MainWindow::offsetComboChanged);
    connect(this->OffsetInput, &QSpinBox::valueChanged, this, &MainWindow::offsetInputChanged);
    connect(this->WidthInput, &QSpinBox::valueChanged, this, &MainWindow::widthChanged);
    connect(this->HeightInput, &QSpinBox::valueChanged, this, &MainWindow::heightChanged);

    connect(this->UncompressedFormatCombo, &QComboBox::currentIndexChanged, this, &MainWindow::uncompressedFormatComboChanged);

    connect(this->Channel1Combo, &QComboBox::currentIndexChanged, this, &MainWindow::channel0ComboChanged);
    connect(this->Channel2Combo, &QComboBox::currentIndexChanged, this, &MainWindow::channel1ComboChanged);
    connect(this->Channel3Combo, &QComboBox::currentIndexChanged, this, &MainWindow::channel2ComboChanged);
    connect(this->Channel4Combo, &QComboBox::currentIndexChanged, this, &MainWindow::channel3ComboChanged);

    connect(this->Channel1Spin, &QSpinBox::valueChanged, this, &MainWindow::bits0SpinChanged);
    connect(this->Channel2Spin, &QSpinBox::valueChanged, this, &MainWindow::bits1SpinChanged);
    connect(this->Channel3Spin, &QSpinBox::valueChanged, this, &MainWindow::bits2SpinChanged);
    connect(this->Channel4Spin, &QSpinBox::valueChanged, this, &MainWindow::bits3SpinChanged);

    connect(&this->m_viewer, &ImageViewer::errorMessage, this->statusbar, &QStatusBar::showMessage);

    this->WidthInput->setMaximum(16384);
    this->WidthInput->setMinimum(4);
    this->WidthInput->setValue(1024);
    m_viewer.setWidth(1024);

    this->HeightInput->setMaximum(16384);
    this->HeightInput->setMinimum(0);
    this->HeightInput->setValue(0);
    m_viewer.setHeight(0);

    this->OffsetInput->setRange(0, 100000);
    this->OffsetInput->setValue(0);
    this->m_viewer.setOffset(0);
    
    #ifdef TEXVIEWER_BPTC
    this->FormatCombo->addItem("BC1", ImageFormat::BC1);
    this->FormatCombo->addItem("BC2", ImageFormat::BC2);
    this->FormatCombo->addItem("BC3", ImageFormat::BC3);
    this->FormatCombo->addItem("BC4", ImageFormat::BC4);
    this->FormatCombo->addItem("BC5", ImageFormat::BC5);
    this->FormatCombo->addItem("BC6HU", ImageFormat::BC6HU);
    this->FormatCombo->addItem("BC6HS", ImageFormat::BC6HS);
    this->FormatCombo->addItem("BC7", ImageFormat::BC7);
    #endif
    #ifdef TEXVIEWER_ASTC
    this->FormatCombo->addItem("ASTC_4x4", ImageFormat::ASTC4x4);
    this->FormatCombo->addItem("ASTC_5x4", ImageFormat::ASTC5x4);
    this->FormatCombo->addItem("ASTC_5x5", ImageFormat::ASTC5x5);
    this->FormatCombo->addItem("ASTC_6x5", ImageFormat::ASTC6x5);
    this->FormatCombo->addItem("ASTC_6x6", ImageFormat::ASTC6x6);
    this->FormatCombo->addItem("ASTC_8x5", ImageFormat::ASTC8x8);
    this->FormatCombo->addItem("ASTC_8x6", ImageFormat::ASTC8x6);
    this->FormatCombo->addItem("ASTC_8x8", ImageFormat::ASTC8x8);
    this->FormatCombo->addItem("ASTC_10x5", ImageFormat::ASTC10x5);
    this->FormatCombo->addItem("ASTC_10x6", ImageFormat::ASTC10x6);
    this->FormatCombo->addItem("ASTC_10x8", ImageFormat::ASTC10x8);
    this->FormatCombo->addItem("ASTC_10x10", ImageFormat::ASTC10x10);
    this->FormatCombo->addItem("ASTC_12x10", ImageFormat::ASTC12x10);
    this->FormatCombo->addItem("ASTC_12x12", ImageFormat::ASTC12x12);
    #endif
    #ifdef TEXVIEWER_ETC
    this->FormatCombo->addItem("ETC", ImageFormat::ETC1);
    this->FormatCombo->addItem("ETC2", ImageFormat::ETC2);
    this->FormatCombo->addItem("ETC2A1", ImageFormat::ETC2A1);
    this->FormatCombo->addItem("ETC2A8", ImageFormat::ETC2A8);
    this->FormatCombo->addItem("EAC1", ImageFormat::EAC1);
    this->FormatCombo->addItem("EAC2", ImageFormat::EAC2);
    #endif

    std::vector<QComboBox*> combos = {
        this->Channel1Combo,
        this->Channel2Combo,
        this->Channel3Combo,
        this->Channel4Combo
    };

    for (QComboBox* x : combos)
    {
        x->addItem("X", ColorChannel::PADDING);
        x->addItem("R", ColorChannel::RED);
        x->addItem("G", ColorChannel::GREEN);
        x->addItem("B", ColorChannel::BLUE);
        x->addItem("A", ColorChannel::ALPHA);
    }

    this->Channel1Combo->setCurrentIndex(this->Channel1Combo->findData(ColorChannel::RED));
    this->Channel1Spin->setRange(0, 63);
    this->Channel1Spin->setValue(8);
    this->Channel2Combo->setCurrentIndex(this->Channel1Combo->findData(ColorChannel::GREEN));
    this->Channel2Spin->setRange(0, 63);
    this->Channel2Spin->setValue(8);
    this->Channel3Combo->setCurrentIndex(this->Channel1Combo->findData(ColorChannel::BLUE));
    this->Channel3Spin->setRange(0, 63);
    this->Channel3Spin->setValue(8);
    this->Channel4Combo->setCurrentIndex(this->Channel1Combo->findData(ColorChannel::PADDING));
    this->Channel4Spin->setRange(0, 63);
    this->Channel4Spin->setValue(0);

    this->UncompressedFormatCombo->addItem("UNORM", UncompressedFormat::UNORM);
    this->UncompressedFormatCombo->addItem("SNORM", UncompressedFormat::SNORM);
    this->UncompressedFormatCombo->addItem("FLOAT", UncompressedFormat::FLOAT);

    m_uncompressedOptions.push_back({this->Channel1Combo, this->Channel1Spin});
    m_uncompressedOptions.push_back({this->Channel2Combo, this->Channel2Spin});
    m_uncompressedOptions.push_back({this->Channel3Combo, this->Channel3Spin});
    m_uncompressedOptions.push_back({this->Channel4Combo, this->Channel4Spin});

    m_viewer.setIsCompressed(true);
    #ifdef TEXVIEWER_BPTC
    this->FormatCombo->setCurrentIndex(0);
    m_viewer.setFormat(ImageFormat::BC1);
    #elif defined(TEXVIEWER_ASTC)
    this->FormatCombo->setCurrentIndex(0);
    m_viewer.setFormat(ImageFormat::ASTC4x4);
    #elif defined(TEXVIEWER_ETC)
    this->FormatCombo->setCurrentIndex(0);
    m_viewer.setFormat(ImageFormat::ETC1);
    #else
    this->FormatOptionsGroup->setTabEnabled(0, false);
    this->FormatOptionsGroup->setCurrentIndex(1);
    setUncompressedLayout();
    m_viewer.setIsCompressed(false);
    this->WidthInput->setMinimum(1);
    #endif

    this->OffsetInput->hide();
    this->OffsetCombo->addItem("None", 0);
    this->OffsetCombo->addItem("DDS Header", 0x80);
    this->OffsetCombo->addItem("DDS+DX10 Header", 0x94);
    this->OffsetCombo->addItem("KTX1 Header", 0x44); // sizeof(KTX1Header) + 0 comment length + 4 bytes
    this->OffsetCombo->addItem("Custom", -1);
}

void
MainWindow::uncompressedComboChangedHandler(int comboIndex, int itemIndex)
{
    setUncompressedLayout();
}

void
MainWindow::uncompressedSpinChangedHandler(int spinIndex, int value)
{
    setUncompressedLayout();
}

void
MainWindow::setUncompressedLayout()
{
    UncompressedLayout layout;
    for (auto& pair : m_uncompressedOptions)
    {
        if (pair.second->value() == 0)
            continue;
        layout.bits.push_back(pair.second->value());
        layout.order.push_back(qvariant_cast<ColorChannel>(pair.first->currentData()));
    }
    layout.format = qvariant_cast<UncompressedFormat>(this->UncompressedFormatCombo->currentData());
    m_viewer.setUncompressedStruct(layout);
}

void
MainWindow::formatTabChanged(int index)
{
    if (index == 0)
    {
        m_viewer.setIsCompressed(true);
        this->WidthInput->setMinimum(4);
    }
    else
    {
        setUncompressedLayout();
        m_viewer.setIsCompressed(false);
        this->WidthInput->setMinimum(1);
    }
}

void
MainWindow::fileOpenTriggered(bool checked)
{
    m_dialog.show();
}

void
MainWindow::fileSelected(const QString& file)
{
    std::string filePathStr = file.toStdString();
    std::filesystem::path filePath(filePathStr);
    m_dialog.hide();
    
    std::ifstream fileHandle(filePath, std::ios::binary | std::ios::ate);
    size_t fileLen = fileHandle.tellg();
    fileHandle.seekg(0, std::ios::beg);

    char* rawData = new char[fileLen];
    fileHandle.read(rawData, fileLen);

    m_viewer.setImageData(rawData, fileLen);

    m_mainWindow->setWindowTitle(QString::fromStdString(std::string("TexViewer ") + TEXVIEWER_VERSION_STR + " - " + filePathStr));
}

void
MainWindow::compressedFormatComboChanged(int index)
{
    const auto format = qvariant_cast<ImageFormat>(this->FormatCombo->itemData(index));
    m_viewer.setFormat(format);
}

void
MainWindow::offsetComboChanged(int index)
{
    const auto value = qvariant_cast<int>(this->OffsetCombo->itemData(index));
    if (value < 0) {
        this->OffsetInput->show();
    } else {
        this->OffsetInput->hide();
        this->OffsetInput->setValue(value);
    }
}

void
MainWindow::offsetInputChanged(int value)
{
    m_viewer.setOffset(value);
}

void
MainWindow::widthChanged(int value)
{
    m_viewer.setWidth(value);
}

void
MainWindow::heightChanged(int value)
{
    m_viewer.setHeight(value);
}

void
MainWindow::channel0ComboChanged(int index)
{
    uncompressedComboChangedHandler(0, index);
}

void
MainWindow::channel1ComboChanged(int index)
{
    uncompressedComboChangedHandler(1, index);
}

void
MainWindow::channel2ComboChanged(int index)
{
    uncompressedComboChangedHandler(2, index);
}

void
MainWindow::channel3ComboChanged(int index)
{
    uncompressedComboChangedHandler(3, index);
}

void
MainWindow::bits0SpinChanged(int value)
{
    uncompressedSpinChangedHandler(0, value);
}

void
MainWindow::bits1SpinChanged(int value)
{
    uncompressedSpinChangedHandler(1, value);
}

void
MainWindow::bits2SpinChanged(int value)
{
    uncompressedSpinChangedHandler(2, value);
}

void
MainWindow::bits3SpinChanged(int value)
{
    uncompressedSpinChangedHandler(3, value);
}

void
MainWindow::uncompressedFormatComboChanged(int index)
{
    setUncompressedLayout();
}
