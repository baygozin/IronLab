#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    //Q_INIT_RESOURCE(application);
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("cccp_project");
    QCoreApplication::setApplicationName("Application_CCCP");
    QCoreApplication::setApplicationVersion(QT_VERSION_STR);
    MainWindow w;
    w.grabGesture(Qt::PanGesture);
    w.grabGesture(Qt::PinchGesture);
    w.show();

    return a.exec();
}
