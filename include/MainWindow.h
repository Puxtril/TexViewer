#pragma once

#include <fstream>
#include <vector>

#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qfiledialog.h>
#include <QtWidgets/qspinbox.h>

#include "ui_MainWindow.h"
#include "ImageViewer.h"


class MainWindow : public QObject, private Ui_MainWindow
{
    Q_OBJECT

    QFileDialog m_dialog;
    ImageViewer m_viewer;
    std::vector<std::pair<QComboBox*, QSpinBox*>> m_uncompressedOptions;

public:
    MainWindow();
    void setupUi(QMainWindow* mainWindow);

private:
    void uncompressedComboChangedHandler(int comboIndex, int itemIndex);
    void uncompressedSpinChangedHandler(int spinIndex, int value);
    // Pulls current data from UI and sends to viewer
    void setUncompressedLayout();

public slots:
    void formatTabChanged(int index);
    void fileOpenTriggered(bool checked = false);
    void fileSelected(const QString& file);
    void compressedFormatComboChanged(int index);
    void offsetComboChanged(int index);
    void offsetInputChanged(int value);
    void widthChanged(int value);

    // Uncompressed widgets
    void channel0ComboChanged(int index);
    void channel1ComboChanged(int index);
    void channel2ComboChanged(int index);
    void channel3ComboChanged(int index);
    void bits0SpinChanged(int value);
    void bits1SpinChanged(int value);
    void bits2SpinChanged(int value);
    void bits3SpinChanged(int value);
};
