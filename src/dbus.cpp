#include "dbus.h"


const QString dbus_utils::errorS = "dbus_error:";

const QString dbus_utils::ErrSystem = "1";
const QString dbus_utils::ErrService = "-1";
const QString ErrNotSupportGpu="2" 
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

QString dbus_utils::parseError(QString errorS){
	QStringList parts = errorS.split(":");
	if (parts.size() >= 2) {
		return parts[1];
	}
	return errorS;
}
