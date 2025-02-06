#include <QThread>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include "dbus.h"

// Worker 类，用于执行后台任务
class Worker : public QObject {
    Q_OBJECT

public:
    explicit Worker(QObject *parent = nullptr) : QObject(parent) {}
   void  executeScript(const QString &scriptCommand, const QStringList &arguments);

public slots:
    void doInstallWork() {
	    // 创建一个方法调用消息
        qDebug() << "Worker: 开始执行任务，线程 ID:" << QThread::currentThreadId();
        dbus_utils::tools("install");
        qDebug() << "脚本执行完成";
        emit workFinished(); // 发送任务完成信号
    }

signals:
    void workFinished();             // 任务完成信号
    void scriptFinished(QByteArray(), QByteArray);

};
