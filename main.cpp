#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{

   QDir().setCurrent(QFileInfo(argv[0]).absolutePath());
   QCoreApplication::addLibraryPath("./plugins");

    Q_INIT_RESOURCE(ResUlo);

    QApplication a(argc, argv);
    MainWindow w;
#ifdef _WIN32
    w.setGeometry(200,200,520,480);
#elif __unix__
    w.setGeometry(200,200,530,490);
#else
    #error Unsupported Operating System
#endif
    w.show();

    return a.exec();
}
