#include "dbus.h"


const QString dbus_utils::errorS = "dbus_error:";
const QString dbus_utils::methodStatus = "status";
const QString dbus_utils::methodInstall = "install";
const QString dbus_utils::openfdeStatusInstalled = "installed\n";
const QString dbus_utils::methodVersionFDE ="version_fde";
const QString dbus_utils::methodVersionCtrl= "version_ctrl";
const QString dbus_utils::methodStop ="stop";
const QString dbus_utils::methodUninstall = "uninstall";

const QString dbus_utils::ErrSystem = "1";
const QString dbus_utils::ErrService = "-1";


const char * dbus_utils::parseError(QString errorS){
	if ( errorS.contains(dbus_utils::errorS)){
		QStringList parts = errorS.split(":");
		if (parts.size() >= 2) {
			int errorCode = parts[1].toInt();
			switch (errorCode) {
				case -1: return  "OpenFDE服务未启动";
				case 1: return   "系统环境错误";
				case 2: return   "不支持的GPU";
				case 3: return   "不支持的内核";
				case 4: return   "您选择退出安装";
				case 5: return   "安装Binder失败";
				case 6: return   "不支持的参数";
				case 7: return   "OpenFDE未安装";
				case 8: return   "请先关闭OpenFDE";
				case 9: return   "未安装curl命令";
				case 10: return  "Pid Max超过65535";
				case 11: return  "网络错误";
				case 100: return  "apt源错误";
				case 143: return  "已取消";
			}
		}
	}
	Logger::log(Logger::DEBUG,"my error code" +errorS.toStdString());
	return "系统错误";
}
