#ifndef _START_WORKER_H_
#define _START_WORKER_H_
#include <QThread>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include "dbus.h"

class StartWorker : public QObject {
    Q_OBJECT

	    
public:
    explicit StartWorker(QObject *parent = nullptr) : QObject(parent) {}

public slots:
    void doStartWork() {
	    // 创建一个方法调用消息
        qDebug() << "Worker: 开始执行任务，线程 ID:" << QThread::currentThreadId();
        int exitCode = QProcess::execute("bash", QStringList() << "/usr/bin/fde_utils"<<"start"<<"shortcut");
        if (exitCode == 0) {
            qDebug() << "fde_utils executed successfully";
        } else {
            qDebug() << "fde_utils execution failed with exit code:" << exitCode;
        }
        emit startEnded(); // 发送任务完成信号
    }

signals:
    void startEnded();             // 任务完成信号
};

#endif
