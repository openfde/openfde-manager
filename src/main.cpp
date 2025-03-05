#include "mainwindow.h"
#include <QApplication>
#include <QLocalServer>
#include <QLocalSocket>
#include <QSharedMemory>
#include <QScreen>
#include <QMenu>
#include "draggable_button.h"
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

	QSharedMemory shared("openfde-manager-single-app");
	if (!shared.create(512, QSharedMemory::ReadWrite)) {
	QLocalSocket socket;
	socket.connectToServer("openfde-manager-single-app");
	if (socket.waitForConnected(500)) {
		socket.write("show");
		socket.waitForBytesWritten(1000);
		socket.disconnectFromServer();
		return 0;
	    }
	}

	QLocalServer server;
	server.removeServer("openfde-manager-single-app");
	server.listen("openfde-manager-single-app");
	QObject::connect(&server, &QLocalServer::newConnection, [&w, &server]() {
		QLocalSocket *socket = server.nextPendingConnection();
		socket->waitForReadyRead(1000);
		w.showNormal();
		w.raise();
		w.activateWindow();
		socket->disconnectFromServer();
	});
    
	QString lang = qgetenv("LANG");
	if (lang.isEmpty()) {
		lang = QLocale::system().name();
	}
	lang = lang.left(2).toLower();

	QString qm = ":/translations/language_en.qm";
	Logger::log(Logger::INFO, "language is "+ lang.toStdString());
	if (lang == "zh") {
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
	QRect screenRect = QGuiApplication::primaryScreen()->geometry();

	DraggableButton *rbtn = new DraggableButton(RIGHT,":/images/right.png",screenRect);
	DraggableButton *lbtn = new DraggableButton(LEFT,":/images/left.png",screenRect);
	rbtn->setWindowOpacity(0.8);
	rbtn->setStyleSheet("background-color: rgba(192,192,192,0.8);");
	lbtn->setWindowOpacity(0.8);
	lbtn->setStyleSheet("background-color: rgba(192,192,192, 0.8);");

	//设置draggableButton的大小为100 * 100
	rbtn->setFixedSize(100, 100);
	lbtn->setFixedSize(100, 100);
	//rbtn 移动到屏幕的右侧中间
	rbtn->move(screenRect.width() - rbtn->width(), screenRect.height() / 2 - rbtn->height() / 2);
	//lbtn 移动到屏幕的左侧中间
	lbtn->move(0, screenRect.height() / 2 - lbtn->height() / 2);
	//设置draggableButton的另一个按钮为lbtn
	rbtn->setOtherButton(lbtn);
	lbtn->setOtherButton(rbtn);
	lbtn->show();
	rbtn->show();

	// Create system tray icon
	QSystemTrayIcon *trayIcon = new QSystemTrayIcon(QIcon(":/images/openfde_icon.png"), &w);
	QMenu *trayMenu = new QMenu(&w);
	QAction *restoreAction = trayMenu->addAction(QObject::tr("恢复窗口"));
	QAction *quitAction = trayMenu->addAction(QObject::tr("退出"));
	QAction *minimizeAction = trayMenu->addAction(QObject::tr("最小化"));

	// Connect actions
	QObject::connect(minimizeAction, &QAction::triggered, &w, &QWidget::hide);
	QObject::connect(restoreAction, &QAction::triggered, &w, &QWidget::showNormal);
	QObject::connect(quitAction, &QAction::triggered, &a, &QApplication::quit);
	// Connect tray icon activation signal
	QObject::connect(trayIcon, &QSystemTrayIcon::activated, [&w](QSystemTrayIcon::ActivationReason reason) {
		if (reason == QSystemTrayIcon::DoubleClick) {
			w.showNormal();
		}
	});

	// Set tray icon menu and show it
	trayIcon->setContextMenu(trayMenu);
	trayIcon->show();

	return a.exec();
}
