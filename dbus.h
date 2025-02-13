#ifndef DBUS_UTILS_H
#define DBUS_UTILS_H

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusReply>
#include <QDebug>
#include <QString>


static const QString dbus_errorS = "error";
static const QString dbus_methodStatus = "status";
static const QString dbus_methodInstall = "install";
static const QString dbus_openfdeStatusInstalled = "installed\n";


class dbus_utils 
{
public:
	static QString construct(){
	    // 创建一个方法调用消息
	    QDBusMessage message = QDBusMessage::createMethodCall(
        "org.example.MyService",          // 服务名
        "/org/example/MyService",         // 对象路径
        "org.example.MyService",          // 接口名
        "Construct"                       // 方法名
	    );


	    // 发送消息并获取回复
	    QDBusConnection bus = QDBusConnection::systemBus();  
	    QDBusReply<QString> reply = bus.call(message);

	    if (reply.isValid()) {
		    qDebug() << "construct 方法调用成功，返回值:" << reply.value();
	    } else {
		    qDebug() << "construct方法调用失败，错误信息:" << reply.error().message();
		    return dbus_errorS;
	    }
	    return reply.value();
	}	
    static QString utils(QString command ){
        QDBusMessage message = QDBusMessage::createMethodCall(
            "org.example.MyService",          // 服务名
            "/org/example/MyService",         // 对象路径
            "org.example.MyService",          // 接口名
            "Utils"                           // 方法名
        );
        QList<QVariant> args;
        args << QVariant::fromValue(QString(command));
        message.setArguments(args);

        int timeout=360000;
        QDBusConnection bus = QDBusConnection::systemBus();  
        QDBusReply<QString> reply = bus.call(message,QDBus::Block,timeout);

        if (reply.isValid()) {
            qDebug() << command <<" 方法调用成功，返回值:" << reply.value();
        } else {
            qDebug() << command <<" 方法调用失败，错误信息:" << reply.error().message();
            return dbus_errorS;
        }
        return reply.value();
	}	
		
	static QString tools(QString command ){
        QDBusMessage message = QDBusMessage::createMethodCall(
            "org.example.MyService",          // 服务名
            "/org/example/MyService",         // 对象路径
            "org.example.MyService",          // 接口名
            "Tools"                           // 方法名
        );

        QList<QVariant> args;
        args << QVariant::fromValue(QString(command));
        message.setArguments(args);

        int timeout=360000;
        QDBusConnection bus = QDBusConnection::systemBus();  
        QDBusReply<QString> reply = bus.call(message,QDBus::Block,timeout);

        if (reply.isValid()) {
            qDebug() << command <<" 方法调用成功，返回值:" << reply.value();
        } else {
            qDebug() << command <<" 方法调用失败，错误信息:" << reply.error().message();
            return dbus_errorS;
        }
        return reply.value();
	}	
};
#endif
