#include "download.h"
#include <QFile>

int FileDownloader:: downloadFile(const QUrl &url, const QString &savePath) {
    QNetworkRequest request(url);
    reply = manager->get(request);

	qDebug()<<"start to download download";
	QEventLoop eventLoop;
	QTimer timer;

    // 设置定时器，超时后退出事件循环
    timer.setSingleShot(true); // 单次触发
    QObject::connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
        // 连接信号和槽
        //connect(reply, &QNetworkReply::finished, this, &FileDownloader::onDownloadFinished);
        //connect(reply, &QNetworkReply::readyRead, this, &FileDownloader::onReadyRead);
	 // 连接信号和槽，当下载完成时退出事件循环
    QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    // 启动定时器 10s timeout
    timer.start(10000);
    // 进入事件循环，等待下载完成或超时
    eventLoop.exec();

    // 检查是否超时
    if (!timer.isActive()) {
        qDebug() << "下载超时或网络故障";
        reply->abort(); // 中止下载
        reply->deleteLater();
        return 1; //超时返回1
    }

    // 停止定时器
    timer.stop();

    // 检查是否有错误发生
    if (reply->error() == QNetworkReply::NoError) {
	    // 将下载的数据保存到文件
        QFile file(savePath);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(reply->readAll());
            file.close();
            qDebug() << "文件下载成功:" << savePath;
            if (!QFile::exists(savePath)) {
                qDebug() << "脚本文件不存在i1:" << savePath;
                return 2;
            }
        } else {
            qDebug() << "无法打开文件进行写入:" << savePath;
            return 2;
        }
    } else {
        qDebug() << "下载失败:" << reply->errorString();
        return 3;
    }

    // 释放reply对象
    reply->deleteLater();
    qDebug("delete download reply");
    return 0;
}

 FileDownloader::~FileDownloader(){
    if  (manager ) {
        delete manager;
    }
 }
