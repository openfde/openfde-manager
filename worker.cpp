#include "worker.h"



void Worker::executeScript(const QString &scriptCommand, const QStringList &arguments) {
    QProcess.start(scriptCommand, arguments);
    if (process.waitForFinished(-1)) {
        QByteArray output = process.readAllStandardOutput();
        QByteArray error = process.readAllStandardError();
        emit scriptFinished(output, error);
    } else {
        qDebug() << "脚本执行超时或出错";
        emit scriptFinished(QByteArray(), QByteArray("Script execution timed out or failed"));
    }
}
