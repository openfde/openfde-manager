
#ifndef EXEC_SHELL_H
#define EXEC_SHELL_H

#include <QFile>
#include <QProcess>
#include <QDebug>
#include "logger.h"

class shellUtils {

public:
	static int  switchToOpenfdeDesktop() 
	{
		QProcess gprocess;
		gprocess.start("bash", QStringList() << "/usr/bin/fde_utils"<<"get_desktop");
		gprocess.waitForFinished(3000);
		QString output(gprocess.readAllStandardOutput());
		Logger::log(Logger::DEBUG,"switch to desktop "+ output.toStdString());
		QProcess::startDetached("wmctrl", QStringList() << "-s" << output);
		return 0;
	}

	static int startOpenfde()
	{
		return  QProcess::execute("bash", QStringList() << "/usr/bin/fde_utils"<<"start");
	}
	static bool stopOpenfde()
	{
		QFile fdeUtils("/usr/bin/fde_utils");
		if (fdeUtils.exists()){
			QProcess *process = new QProcess();
			process->start("bash", QStringList() << "/usr/bin/fde_utils"<<"stop");
			process->waitForFinished(-1);
			QString output(process->readAllStandardOutput());
			Logger::log(Logger::INFO, QString("fde_utils output: %1").arg(output).toStdString());
			return true;
		}
		return false;
	}

	static bool isOpenfdeClosed()
        {
		QFile fdeUtils("/usr/bin/fde_utils");
		if (fdeUtils.exists()){
			QProcess *process = new QProcess();
			process->start("bash", QStringList() << "/usr/bin/fde_utils"<<"status");
			process->waitForFinished(-1);
			//获取waitForFinished返回值
			int exitCode = process->exitCode();
			qDebug()<<exitCode <<"exit code";
			if (exitCode == 1) {
				return false;
			}else {
				qDebug()<<exitCode <<"return true";
				return true;
			}
		}
		return false;
	}
};
#endif
