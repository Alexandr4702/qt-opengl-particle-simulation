#include <QApplication>
#include <QSurfaceFormat>
#include "qmainwindows_.h"

int main(int argc,char **argv )
{
    QSurfaceFormat format;
    format.setVersion(4, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication a(argc,argv);

    Qmainwindows_ MainWindow;
    MainWindow.show();

    return a.exec();
}
