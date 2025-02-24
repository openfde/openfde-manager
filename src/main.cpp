#include "mainwindow.h"
#include <QApplication>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WindowActivateFilter filter;
    MainWindow w;
    w.installEventFilter(&filter);
    w.setWindowIcon(QIcon(":/images/openfde_icon.png"));
    w.show();

    
    QString lang = qgetenv("LANG");
    if (lang.isEmpty()) {
      lang = QLocale::system().name();
    }
    lang = lang.left(2).toLower();

    QString qm = ":/translations/language_en.qm";
    if (lang != "zh") {
	qm = ":/translations/language_zh.qm";
    }
    QTranslator translator;
    if (!QFile::exists(qm)){
	    Logger::log(Logger::ERROR,"qm not exist");
	    return 0; 
    }
    if (translator.load(qm)) {
	    a.installTranslator(&translator);
    }else {
	    Logger::log(Logger::ERROR,"qm load failed");
	    return 0;
    }

      // Create system tray icon
    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(QIcon(":/images/openfde_icon.png"), &w);
    QMenu *trayMenu = new QMenu(&w);
    QAction *restoreAction = trayMenu->addAction(QObject::tr("恢复窗口"));
    QAction *quitAction = trayMenu->addAction(QObject::tr("退出"));

    // Connect actions
    QObject::connect(restoreAction, &QAction::triggered, &w, &QWidget::showNormal);
    QObject::connect(quitAction, &QAction::triggered, &a, &QApplication::quit);

    // Set tray icon menu and show it
    trayIcon->setContextMenu(trayMenu);
    trayIcon->show();

    return a.exec();
}
