#include "installwidget.h"

InstallWidget::InstallWidget(QWidget *parent)
    : QWidget(parent), currentProgress(0)
{
    // 设置窗口大小
    this->setFixedSize(400, 200);
    this->download=false;
    this->extra=false;
    this->install=false;

    // 创建布局
    QVBoxLayout *layout = new QVBoxLayout(this);

    // 创建进度条
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100); // 设置进度范围
    progressBar->setValue(0);      // 初始值为 0
    layout->addWidget(progressBar);

    // 创建状态标签
    statusLabel = new QLabel("准备中...", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(statusLabel);

    // 创建定时器
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &InstallWidget::updateProgress);
    timer->start(1000); // 每秒更新一次进度
}

InstallWidget::~InstallWidget()
{
}

void InstallWidget::updateProgress()
{
    // 模拟安装进度
    currentProgress += 10; // 每次增加 10%
    if (currentProgress > 100) {
	if (this->recyleNum == 0 ){
		download = true;
	}
	if (this->recyleNum == 1 ){
		extra = true;
	}
	if (this->recyleNum == 2 ){
		install = true;
	}else {
		this->recyleNum++;
		currentProgress = 0 ;
	}
    }

    // 更新进度条
    progressBar->setValue(currentProgress);

    if (!download && !extra && !install) {
        statusLabel->setText("下载中... " + QString::number(currentProgress) + "%");
    }else if (download && !extra && !install) {
        statusLabel->setText("解压中... " + QString::number(currentProgress) + "%");
    }else if (download && extra && !install) {
        statusLabel->setText("安装中... " + QString::number(currentProgress) + "%");
    } else {
	timer->stop(); // 进度完成后停止定时器
        statusLabel->setText("安装完成！");
	this->close(); // 关闭窗口
	    this->deleteLater(); // 安全销毁窗口
    }
}
