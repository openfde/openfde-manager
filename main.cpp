#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	QApplication::setSetuidAllowed(true);
    QApplication a(argc, argv);
    WindowActivateFilter filter;
    MainWindow w;
    w.installEventFilter(&filter);
    w.setWindowIcon(QIcon(":/images/openfde_icon.png"));
    w.show();
    return a.exec();
}
