#include "download.h"
#include "logger.h"
#include <QFile>

int FileDownloader:: downloadFile(const QUrl &url, const QString &savePath) {
	QNetworkRequest request(url);
	reply = manager->get(request);

	Logger::log(Logger::INFO,"start to download get-openfde.sh");
	QEventLoop eventLoop;
	QTimer timer;

	// 设置定时器，超时后退出事件循环
	timer.setSingleShot(true); // 单次触发
	QObject::connect(&timer, &QTimer::timeout, &eventLoop, &QEventLoop::quit);
	QObject::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
	// 启动定时器 10s timeout
	timer.start(10000);
	// 进入事件循环，等待下载完成或超时
	eventLoop.exec();

	// 检查是否超时
	if (!timer.isActive()) {
		Logger::log(Logger::ERROR,"timeout after 10s or network failure for downloading get-openfde.sh");
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
			Logger::log(Logger::INFO,QString("download %1 successfully").arg(savePath).toStdString());
			if (!QFile::exists(savePath)) {
				Logger::log(Logger::ERROR, QString("create file failed %1").arg(savePath).toStdString());
				return 2;
			}
		} else {
			Logger::log(Logger::ERROR,QString("file not allowed to write %1").arg(savePath).toStdString());
			return 2;
			}
	} else {
		Logger::log(Logger::ERROR, "Download get-openfde.sh failed ");
		return 3;
	}
	// 释放reply对象
	reply->deleteLater();
	return 0;
}

 FileDownloader::~FileDownloader(){
    if  (manager ) {
        delete manager;
    }
 }
