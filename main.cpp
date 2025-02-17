#include "mainwindow.h"
#include <QApplication>
#include <QMenu>
#include <QSystemTrayIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WindowActivateFilter filter;
    MainWindow w;
    w.installEventFilter(&filter);
    w.setWindowIcon(QIcon(":/images/openfde_icon.png"));
    w.show();
      // Create system tray icon
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(QIcon(":/images/openfde_icon.png"), &w);
    QMenu *trayMenu = new QMenu(&w);
    QAction *minimizeAction = trayMenu->addAction("Minimize");
    QAction *restoreAction = trayMenu->addAction("Restore");
    QAction *quitAction = trayMenu->addAction("Quit");

    // Connect actions
    QObject::connect(minimizeAction, &QAction::triggered, &w, &QWidget::hide);
    QObject::connect(restoreAction, &QAction::triggered, &w, &QWidget::showNormal);
    QObject::connect(quitAction, &QAction::triggered, &a, &QApplication::quit);

    // Set tray icon menu and show it
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();

    return a.exec();
}
