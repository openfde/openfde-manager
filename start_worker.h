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
	    process = new QProcess();
        process->start("/usr/bin/fde_utils", QStringList() << "start");
        process->waitForFinished(-1);
        QString output(process->readAllStandardOutput());
        qDebug() << "fde_utils output:" << output;
        emit startEnded(); // 发送任务完成信号
	    delete process;
    }

signals:
    void startEnded();             // 任务完成信号
private:
    QProcess *process;
};
