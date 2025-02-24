#ifndef DBUS_UTILS_H
#define DBUS_UTILS_H

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDebug>
#include "logger.h"
#include <QString>


static const QString dbus_methodStatus = "status";
static const QString dbus_methodInstall = "install";
static const QString dbus_openfdeStatusInstalled = "installed\n";


class dbus_utils 
{
	
public:
	static const QString errorS; 
	static const QString ErrService; 
	static const QString ErrSystem;  
	static const QString ErrNotSupportGpu; 
	static const QString ErrNotSupportKernel;
	static const QString ErrChooseToExitByUser;
	static const QString ErrBinderInstallFailed;
	static const QString ErrInvalidArgs;
	static const QString ErrFDENotInstalled;
	static const QString ErrStopNeedFirst;
	static const QString ErrCurlNotInstalled;
	static const QString ErrPidMaxOver;
	static const QString ErrNetworkError;
	static const QString ErrSystemApt100;

	static const char* parseError(QString);
	static QString construct(){
		// 创建一个方法调用消息
		QDBusMessage message = QDBusMessage::createMethodCall(
		"com.openfde.Manager",          // 服务名
		"/com/openfde/Manager",         // 对象路径
		"openfde.service",          // 接口名
		"Construct"                       // 方法名
		 );


		// 发送消息并获取回复
		QDBusConnection bus = QDBusConnection::systemBus();  
		QDBusReply<QString> reply = bus.call(message);

		if (reply.isValid()) {
			Logger::log(Logger::INFO, QString("call the get_fde.sh construction successful, reply value is ").arg(reply.value()).toStdString());
		} else {
			Logger::log(Logger::ERROR,QString("call the get_fde.sh construction failed, reply message is").arg(reply.error().message()).toStdString());
			return errorS;
		}
		return reply.value();
	}	

	static QString utils(QString command ){
		QDBusMessage message = QDBusMessage::createMethodCall(
		"com.openfde.Manager",          // 服务名
		"/com/openfde/Manager",         // 对象路径
		"openfde.service",          // 接口名
		"Utils"                           // 方法名
		);
		QList<QVariant> args;
		args << QVariant::fromValue(QString(command));
		message.setArguments(args);

		int timeout=360000;
		QDBusConnection bus = QDBusConnection::systemBus();  
		QDBusReply<QString> reply = bus.call(message,QDBus::Block,timeout);

		if (reply.isValid()) {
			Logger::log(Logger::INFO, QString("%1 call utils successful, reply value is %2").arg(command).arg(reply.value()).toStdString());
		} else {
			Logger::log(Logger::ERROR, QString("%1 called utils failed : %2").arg(command).arg(reply.error().message()).toStdString());
			return errorS;
		}
		return reply.value();
	}	
		
	static QString tools(QString command ){
		QDBusMessage message = QDBusMessage::createMethodCall(
		"com.openfde.Manager",          // 服务名
		"/com/openfde/Manager",         // 对象路径
		"openfde.service",          // 接口名
		"Tools"                           // 方法名
		);

		QList<QVariant> args;
		args << QVariant::fromValue(QString(command));
		message.setArguments(args);

		int timeout=360000;
		QDBusConnection bus = QDBusConnection::systemBus();  
		QDBusReply<QString> reply = bus.call(message,QDBus::Block,timeout);

		if (reply.isValid()) {
			Logger::log(Logger::INFO, QString("%1 call tools successful, reply value is %2").arg(command).arg(reply.value()).toStdString());
			if (reply.value().contains("FailedExitCode")) {
				QStringList parts = reply.value().split(" ");
				if (parts.size() >= 2) {
					qDebug()<<"errno is "<<parts[1];
					return errorS +  parts[1];
				}
				return errorS+ErrSystem;
			}
			
		} else {
			Logger::log(Logger::ERROR, QString("%1 called failed : %2").arg(command).arg(reply.error().message()).toStdString());
			return errorS + ErrService;
		}
		return reply.value();
	}	
};

#endif
