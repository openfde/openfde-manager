#ifndef _START_WORKER_H_
#define _START_WORKER_H_
#include <QThread>
#include <QDebug>
#include <QProcess>
#include "logger.h"
#include "shellUtils.h"

class StartWorker : public QObject {
	Q_OBJECT

	    
public:
	explicit StartWorker(QObject *parent = nullptr) : QObject(parent) {}
	private:
	bool hasWinOnDesktop(const QString& originalDesktop) {
		QProcess proc;
		proc.start("wmctrl", QStringList() << "-l");
		proc.waitForFinished();
		QString output = QString::fromLocal8Bit(proc.readAllStandardOutput());

		QStringList windows = output.split("\n");
		for (const QString& window : windows) {
			QStringList fields = window.split(QRegExp("\\s+"));
			if (fields.length() > 2 && fields[1] == originalDesktop) {
				return true;
			}
		}
		return false;
	}

	void moveWindowsBetweenDesktops(const QString& originalDesktop, const QString& targetDesktop) {
		QProcess proc;
		proc.start("wmctrl", QStringList() << "-l");
		proc.waitForFinished();
		QString output = QString::fromLocal8Bit(proc.readAllStandardOutput());

		QStringList windows = output.split("\n");
		for (const QString& window : windows) {
			QStringList fields = window.split(QRegExp("\\s+"));
			if (fields.length() > 2 && fields[1] == originalDesktop) {
				QString windowId = fields[0];
				QProcess::execute("wmctrl", QStringList() << "-ir" << windowId << "-t" << targetDesktop);
			}
		}
	}
	void chooseDesktop() {
		// Get number of desktops
		QProcess wmctrlProcess;
		wmctrlProcess.start("wmctrl", QStringList() << "-d");
		wmctrlProcess.waitForFinished();
		QString output = QString::fromLocal8Bit(wmctrlProcess.readAllStandardOutput());
		int numOfDesktop = output.split("\n").count() - 1;

		// Get current desktop
		int currentDesktop = 0;
		QStringList lines = output.split("\n");
		for (const QString& line : lines) {
			if (line.contains("*")) {
			QStringList fields = line.split(QRegExp("\\s+"));
			currentDesktop = fields[0].toInt();
			break;
			}
		}
		int fdeDesktopInt = currentDesktop;

		if (numOfDesktop == 1) { //only 1 desktop
			QProcess::execute("wmctrl", QStringList() << "-n" << "2");
			fdeDesktopInt = 1;
			QProcess::execute("wmctrl", QStringList() << "-s" << QString::number(fdeDesktopInt));
			return;
		} else { //condition: two or more desktops
			// Check desktops after current desktop
			for (int i = currentDesktop ; i <= numOfDesktop -1; i++) {
				if (!hasWinOnDesktop(QString::number(i))) {
					fdeDesktopInt = i;
					QProcess::execute("wmctrl", QStringList() << "-s" << QString::number(fdeDesktopInt));
					return;
				}
			}

			// Check desktops before current desktop
			for (int i = currentDesktop ; i > 0; i--) {
				if (!hasWinOnDesktop(QString::number(i))) {
					fdeDesktopInt = i;
					QProcess::execute("wmctrl", QStringList() << "-s" << QString::number(fdeDesktopInt));
					return;
				}
			}
			//如果所有桌面都有窗口，则在当前桌面新建一个桌面
			int newNumOfDesktop = numOfDesktop + 1;
			QProcess::execute("wmctrl", QStringList() << "-n" << QString::number(newNumOfDesktop));
			for (int i = newNumOfDesktop - 1; i > currentDesktop + 1; i--) {
				moveWindowsBetweenDesktops(QString::number(i-1), QString::number(i));
			}
			QProcess::execute("wmctrl", QStringList() << "-s" << QString::number(currentDesktop+1));
			return;
		}
	}

public slots:
	void doStartWork() {
		// 创建一个方法调用消息
		chooseDesktop();
		int exitCode = shellUtils::startOpenfde();
		if (exitCode == 0) {
			Logger::log(Logger::DEBUG,  "start openfde executed successfully");
		} else {
			Logger::log(Logger::ERROR,  "start openfde executed failed with exit code " + exitCode);
		}
		emit startEnded(); // 发送任务完成信号
	}

signals:
	void startEnded();             // 任务完成信号
};

#endif
