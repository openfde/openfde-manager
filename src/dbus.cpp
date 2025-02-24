#include "dbus.h"


const QString dbus_utils::errorS = "dbus_error:";

const QString dbus_utils::ErrSystem = "1";
const QString dbus_utils::ErrService = "-1";
const QString ErrNotSupportGpu="2";
const QString ErrNotSupportKernel="3";
const QString ErrChooseToExitByUser="4";
const QString ErrBinderInstallFailed="5";
const QString ErrInvalidArgs="6";
const QString ErrFDENotInstalled="7";
const QString ErrStopNeedFirst="8";
const QString ErrCurlNotInstalled="9";
const QString ErrPidMaxOver="10";
const QString ErrNetworkError="11";
const QString ErrSystemApt100="100";


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
			}
		}
	}
	Logger::log(Logger::DEBUG,"my error code" +errorS.toStdString());
	return "系统错误";
}
