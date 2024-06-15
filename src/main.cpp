#include <QtWidgets/qmainwindow.h>
#include <QtWidgets/qapplication.h>
#include <QtWidgets/qstylefactory.h>
#include <QtGui/qpalette.h>

#include "MainWindow.h"

int
main(int argc, char** argv)
{
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));

    MainWindow windowLayout;
    QMainWindow window;
    windowLayout.setupUi(&window);

    window.show();

    return app.exec();
}