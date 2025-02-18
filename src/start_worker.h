#ifndef _START_WORKER_H_
#define _START_WORKER_H_
#include <QThread>
#include <QDebug>
#include <QProcess>
#include "logger.h"

class StartWorker : public QObject {
	Q_OBJECT

	    
public:
	explicit StartWorker(QObject *parent = nullptr) : QObject(parent) {}

public slots:
	void doStartWork() {
		// 创建一个方法调用消息
		int exitCode = QProcess::execute("bash", QStringList() << "/usr/bin/fde_utils"<<"start"<<"shortcut");
		if (exitCode == 0) {
			Logger::log(Logger::DEBUG,  "fde_utils executed successfully");
		} else {
			Logger::log(Logger::ERROR,  "fde_utils executed failed with exit code " + exitCode);
		}
		emit startEnded(); // 发送任务完成信号
	}

signals:
	void startEnded();             // 任务完成信号
};

#endif
