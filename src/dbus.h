#ifndef DBUS_UTILS_H
#define DBUS_UTILS_H

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDebug>
#include "logger.h"
#include <QString>




class dbus_utils 
{
	
public:
	static const QString methodStatus;
	static const QString methodInstall;
	static const QString methodVersionFDE;
	static const QString methodVersionCtrl;
	static const QString methodStop;
	static const QString methodUninstall;
	static const QString openfdeStatusInstalled;
	static const QString methodSecurityQuery ;
	static const QString methodSecurityDisable;

	
	static const QString statusSecurityEnable;

	static const QString errorS; 
	static const QString ErrService; 
	static const QString ErrTimeout;
	static const QString ErrSystem;  

	static const char* parseError(QString);
	static QString construct(){
		QDBusMessage message = connectDBus("Construct");

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
	static QDBusMessage connectDBus(const char* methodName){
		return  QDBusMessage::createMethodCall(
		"com.openfde.Manager",          // 服务名
		"/com/openfde/Manager",         // 对象路径
		"openfde.service",          // 接口名
		methodName                 // 方法名
		);

	}

	static QString security(QString command ){
		QDBusMessage message = connectDBus("Security");
		QList<QVariant> args;
		args << QVariant::fromValue(QString(command));
		message.setArguments(args);

		int timeout=4000;
		QDBusConnection bus = QDBusConnection::systemBus();  
		QDBusReply<QString> reply = bus.call(message,QDBus::Block,timeout);

		if (reply.isValid()) {
			Logger::log(Logger::INFO, QString("%1 call security successful, reply value is %2").arg(command).arg(reply.value()).toStdString());
		} else {
			Logger::log(Logger::ERROR, QString("%1 called security failed : %2").arg(command).arg(reply.error().message()).toStdString());
			return errorS;
		}
		return reply.value();
	}

	static QString clear(QString homeDir ){
		QDBusMessage message = connectDBus("Clear");
		QList<QVariant> args;
		args << QVariant::fromValue(QString(homeDir));
		message.setArguments(args);

		int timeout=4000;
		QDBusConnection bus = QDBusConnection::systemBus();  
		QDBusReply<QString> reply = bus.call(message,QDBus::Block,timeout);

		if (reply.isValid()) {
			Logger::log(Logger::INFO, QString("%1 call clear successful, reply value is %2").arg(homeDir).arg(reply.value()).toStdString());
		} else {
			Logger::log(Logger::ERROR, QString("%1 called clear failed : %2").arg(homeDir).arg(reply.error().message()).toStdString());
			return errorS;
		}
		return reply.value();
	}

	static QString utils(QString command ){
		QDBusMessage message = connectDBus("Utils");
		QList<QVariant> args;
		args << QVariant::fromValue(QString(command));
		message.setArguments(args);

		int timeout=4000;
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
		QDBusMessage message = connectDBus("Tools");
		QList<QVariant> args;
		args << QVariant::fromValue(QString(command));
		message.setArguments(args);

		int timeout=60000; //1 min
		QDBusConnection bus = QDBusConnection::systemBus();  

		if (command.contains("install") && !command.contains("uninstall") ) {
			timeout=600000; //10 min
			Logger::log(Logger::INFO, QString("tools command is %1").arg(command).toStdString());
		}

		QDBusReply<QString> reply = bus.call(message,QDBus::Block,timeout);
		if (reply.error().type() == QDBusError::NoReply) {
			Logger::log(Logger::ERROR, QString("%1 called tools no reply perhaps timeout").arg(command).toStdString());
			return errorS + ErrTimeout;
		}

		if (reply.isValid()) {
			Logger::log(Logger::INFO, QString("%1 call tools successful, reply value is %2").arg(command).arg(reply.value()).toStdString());
			if (reply.value().contains("FailedExitCode")) {
				QStringList parts = reply.value().split(" ");
				if (parts.size() >= 2) {
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
