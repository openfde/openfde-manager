
#ifndef EXEC_SHELL_H
#define EXEC_SHELL_H
#include <QFile>
#include <QProcess>


class shellUtils
{
    public:
        static bool is_openfde_closed()
        {
            QFile fdeUtils("/usr/bin/fde_utils");
            if (fdeUtils.exists()){
            QProcess *process = new QProcess();
            process->start("bash", QStringList() << "/usr/bin/fde_utils"<<"status");
            process->waitForFinished(-1);
            //获取waitForFinished返回值
            int exitCode = process->exitCode();
                if (exitCode == 1) {
                    return false;
                }
            }
            return true;
        }
}
#endif