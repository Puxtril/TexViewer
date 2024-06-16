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
    Ui_MainWindow::setupUi(mainWindow);
    m_viewer.setWidget(this->ImagePreviewArea);

    mainWindow->setWindowTitle(mainWindow->windowTitle() + " " + TEXVIEWER_VERSION_STR);

    ClampedSpinbox* newSpinbox = new ClampedSpinbox({2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384});
    this->WidthInput->hide();
    this->ResolutionLayout->replaceWidget(this->WidthInput, newSpinbox);
    this->WidthInput = newSpinbox;

    connect(this->FormatOptionsGroup, &QTabWidget::currentChanged, this, &MainWindow::formatTabChanged);
    connect(this->actionOpen, &QAction::triggered, this, &MainWindow::fileOpenTriggered);
    connect(&m_dialog, &QFileDialog::fileSelected, this, &MainWindow::fileSelected);
    connect(this->FormatCombo, &QComboBox::currentIndexChanged, this, &MainWindow::compressedFormatComboChanged);
    connect(this->OffsetCombo, &QComboBox::currentIndexChanged, this, &MainWindow::offsetComboChanged);
    connect(this->OffsetInput, &QSpinBox::valueChanged, this, &MainWindow::offsetInputChanged);
    connect(this->WidthInput, &QSpinBox::valueChanged, this, &MainWindow::widthChanged);

    connect(this->Channel1Combo, &QComboBox::currentIndexChanged, this, &MainWindow::channel0ComboChanged);
    connect(this->Channel2Combo, &QComboBox::currentIndexChanged, this, &MainWindow::channel1ComboChanged);
    connect(this->Channel3Combo, &QComboBox::currentIndexChanged, this, &MainWindow::channel2ComboChanged);
    connect(this->Channel4Combo, &QComboBox::currentIndexChanged, this, &MainWindow::channel3ComboChanged);

    connect(this->Channel1Spin, &QSpinBox::valueChanged, this, &MainWindow::bits0SpinChanged);
    connect(this->Channel2Spin, &QSpinBox::valueChanged, this, &MainWindow::bits1SpinChanged);
    connect(this->Channel3Spin, &QSpinBox::valueChanged, this, &MainWindow::bits2SpinChanged);
    connect(this->Channel4Spin, &QSpinBox::valueChanged, this, &MainWindow::bits3SpinChanged);

    this->WidthInput->setMaximum(16384);
    this->WidthInput->setMinimum(1);
    this->WidthInput->setValue(1024);
    m_viewer.setWidth(1024);

    this->OffsetInput->setRange(0, 100000);
    this->OffsetInput->setValue(0);
    this->m_viewer.setOffset(0);
    
    this->FormatCombo->addItem("BC1", ImageFormat::BC1);
    this->FormatCombo->addItem("BC2", ImageFormat::BC2);
    this->FormatCombo->addItem("BC3", ImageFormat::BC3);
    this->FormatCombo->addItem("BC4", ImageFormat::BC4);
    this->FormatCombo->addItem("BC5", ImageFormat::BC5);
    this->FormatCombo->addItem("BC6HU", ImageFormat::BC6HU);
    this->FormatCombo->addItem("BC6HS", ImageFormat::BC6HS);
    this->FormatCombo->addItem("BC7", ImageFormat::BC7);
    this->FormatCombo->setCurrentIndex(0);
    m_viewer.setFormat(ImageFormat::BC1);

    std::vector<QComboBox*> combos = {
        this->Channel1Combo,
        this->Channel2Combo,
        this->Channel3Combo,
        this->Channel4Combo
    };
    for (QComboBox* x : combos)
    {
        x->addItem("-", ColorChannel::PADDING);
        x->addItem("R", ColorChannel::RED);
        x->addItem("G", ColorChannel::GREEN);
        x->addItem("B", ColorChannel::BLUE);
        x->addItem("A", ColorChannel::ALPHA);
    }

    this->Channel1Combo->setCurrentIndex(this->Channel1Combo->findData(ColorChannel::RED));
    this->Channel1Spin->setRange(1, 63);
    this->Channel1Spin->setValue(8);
    this->Channel2Combo->setCurrentIndex(this->Channel1Combo->findData(ColorChannel::GREEN));
    this->Channel2Spin->setRange(1, 63);
    this->Channel2Spin->setValue(8);
    this->Channel3Combo->setCurrentIndex(this->Channel1Combo->findData(ColorChannel::BLUE));
    this->Channel3Spin->setRange(1, 63);
    this->Channel3Spin->setValue(8);
    this->Channel4Combo->setCurrentIndex(this->Channel1Combo->findData(ColorChannel::PADDING));
    this->Channel4Spin->setRange(1, 63);
    this->Channel4Spin->setValue(0);

    m_uncompressedOptions.push_back({this->Channel1Combo, this->Channel1Spin});
    m_uncompressedOptions.push_back({this->Channel2Combo, this->Channel2Spin});
    m_uncompressedOptions.push_back({this->Channel3Combo, this->Channel3Spin});
    m_uncompressedOptions.push_back({this->Channel4Combo, this->Channel4Spin});

    this->OffsetInput->hide();
    this->OffsetCombo->addItem("None");
    this->OffsetCombo->addItem("DDS Header");
    this->OffsetCombo->addItem("DDS+DX10 Header");
    this->OffsetCombo->addItem("Custom");
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
    m_viewer.setUncompressedStruct(layout);
}

void
MainWindow::formatTabChanged(int index)
{
    if (index == 0)
    {
        m_viewer.setIsCompressed(true);
    }
    else
    {
        setUncompressedLayout();
        m_viewer.setIsCompressed(false);
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
}

void
MainWindow::compressedFormatComboChanged(int index)
{
    ImageFormat format = qvariant_cast<ImageFormat>(this->FormatCombo->itemData(index));
    m_viewer.setFormat(format);
}

void
MainWindow::offsetComboChanged(int index)
{
    switch(index)
    {
    case(0):
        this->OffsetInput->hide();
        this->OffsetInput->setValue(0);
        break;
    case(1):
        this->OffsetInput->hide();
        this->OffsetInput->setValue(128);
        break;
    case(2):
        this->OffsetInput->hide();
        this->OffsetInput->setValue(148);
        break;
    case(3):
        this->OffsetInput->show();
        break;
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

