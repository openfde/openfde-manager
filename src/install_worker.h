#include <QThread>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QFile>
#include <QDebug>
#include <QTimer>
#include <QProcess>
#include "dbus.h"
#include "logger.h"

// Worker 类，用于执行后台任务
class Worker : public QObject {
    Q_OBJECT

public:
    explicit Worker(QObject *parent = nullptr) : QObject(parent) {}

public slots:
    void doInstallWork() {
	// 创建一个方法调用消息
	Logger::log(Logger::INFO,"start to run installing worker");
	QString retStatus = dbus_utils::tools("install");
	if ( retStatus.contains(dbus_utils::errorS)){
		Logger::log(Logger::ERROR,"installing procedure occur error"+ retStatus.toStdString());
		emit workFinishedErr(retStatus);
		return ;
	}
	Logger::log(Logger::INFO,"installing procedure worker end");
	emit workFinished(); // 发送任务完成信号
    }

signals:
    void workFinished();             // 任务完成信号
    void workFinishedErr(QString );             // 任务完成信号
};
