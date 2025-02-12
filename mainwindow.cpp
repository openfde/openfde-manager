#include "mainwindow.h"
#include "dbus.h"
#include <unistd.h>
#include <QRegularExpression>
#include <QPixmap>
#include <QPainter>
#include <QPolygon>
#include <QTimer>
#include <QMessageBox>
#include <QIcon>
#include <QHBoxLayout>
#include <QCoreApplication>
#include <QProcess>
#include <QFile>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 移除默认的标题栏
    this->setWindowFlags(Qt::FramelessWindowHint);


    // 设置窗口大小
    this->setFixedSize(800, 600);


    // 创建自定义标题栏
    createTitleBar();

    centralWidget = new QWidget(this);
    centralWidget->resize(this->width(), this->height()-30);
    centralLayout = new QVBoxLayout(centralWidget);
    centralLayout->setSpacing(10);
    centralWidget->setLayout(centralLayout);
    centralWidget->move(0,30);

    imageLabel = new QLabel(this);
    QPixmap pixmap(":/images/openfde.png"); // 图片路径，需要将图片添加到资源文件中
    imageLabel->setPixmap(pixmap.scaled(this->size(), Qt::KeepAspectRatioByExpanding));
    imageLabel->setGeometry(0, 30, this->width(), this->height() - 30); // 留出标题栏的空间
    imageLabel->lower();

    btn = new CircleWidgetWithButton(this);
    btn->show();
    btn->move(300,200);

    // 连接启动按钮点击事件
    connect(this, &MainWindow::imageSignal, this, &MainWindow::showImage);
    connect(btn, &CircleWidgetWithButton::sendMessage, this, &MainWindow::onMessageReceived);
}

void MainWindow::onMessageReceived( const QString & string) {
    qDebug() << "ReceiverClass: Received message:" << string;
    installWorker = new Worker();
	if (string  == button_start) {
	    imageLabel->hide();
	    btn->hide();
	    QString filepath = "/usr/bin/get_fde.sh";
	    QFile getfde(filepath);
        //check the exist of the primary script get_fde.sh
	    if (!getfde.exists()){
            QUrl url("http://phyvirt.openfde.com/getopenfde/get-openfde.sh"); // 下载文件的 URL
            QString savePath("/tmp/get-openfde.sh");
            downloader = new FileDownloader();
            int ret = downloader->downloadFile(url, savePath);
            if ( ret != 0 ){ //download file failed
                 // 创建状态标签
                QLabel *status = new QLabel("Error: network or disk error", this);
                status->setAlignment(Qt::AlignCenter);
                centralLayout->addWidget(status);
                return ;
            }
            //construct /usr/bin/get_fde.sh
		    QString retS = dbus_utils::construct();
            if (retS == dbus_errorS) {
                  // 创建状态标签
                QLabel *status = new QLabel("Error: openFDE Service not started.", this);
                status->setAlignment(Qt::AlignCenter);
                centralLayout->addWidget(status);
                return ;
            }
	    }
    
        workThread = new QThread();
        // 将 Worker 移动到子线程
        installWorker->moveToThread(workThread);
        initProgress();
        // 连接 Worker 的信号和槽
        // QObject::connect(workThread, &QThread::started, installWorker, &Worker::executeScript);
        // QObject::connect(installWorker, &Worker::scriptFinished, workThread, &MainWindow::onScriptFinished);
	}
}

void MainWindow::showImage(const QString &imagePath) {
    // 加载图片
    QPixmap pixmap(imagePath);
    if (pixmap.isNull()) {
        imageLabel->setText("Failed to load image!"); // 如果图片加载失败，显示错误信息
    } else {
        centralWidget->resize(this->width(),this->height()-30);
        centralWidget->move(0,30);
        imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio)); // 缩放图片并显示
        imageLabel->show();
    }
}

// Add a new slot to handle the script completion
void MainWindow::onScriptFinished(const QByteArray &output, const QByteArray &error) {
    if (!error.isEmpty()) {
        qDebug() << "on script finished 脚本执行错误：" << error;
    } else {
        qDebug() << "on script finished  脚本执行结果：" << output;
    }
}



MainWindow::~MainWindow()
{
}

void MainWindow::createTitleBar()
{
    // 创建标题栏容器
    QWidget *titleBar = new QWidget(this);
    titleBar->setGeometry(0, 0, this->width(), 30); // 设置标题栏大小
    titleBar->setStyleSheet("background-color: #ffffff;"); // 设置标题栏背景颜色为白色
							   //
    QHBoxLayout *layout = new QHBoxLayout(titleBar);
    layout->setContentsMargins(5, 0, 5, 0); // 设置布局边距

    // 添加标题
    QLabel *titleLabel = new QLabel("FDE Manager", titleBar); // 创建标题标签
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333333;"); // 设置标题样式
    layout->addWidget(titleLabel); // 将标题添加到布局左侧

    // 创建设置按钮
    settingsButton = new QPushButton(titleBar);
    settingsButton->setGeometry(this->width() - 120, 5, 20, 20); // 设置按钮位置和大小
    settingsButton->setIcon(QIcon(":/images/settings.png")); // 使用资源文件中的图标
    settingsButton->setIconSize(QSize(16, 16)); // 设置图标大小
    settingsButton->setStyleSheet("border: none; background-color: transparent;"); // 设置按钮样式
    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsButtonClicked);

    // 创建最小化按钮
    minimizeButton = new QPushButton(titleBar);
    minimizeButton->setGeometry(this->width() - 80, 5, 20, 20); // 设置按钮位置和大小
    minimizeButton->setIcon(QIcon(":/images/minimize.png")); // 使用资源文件中的图标
    minimizeButton->setIconSize(QSize(16, 16)); // 设置图标大小
    minimizeButton->setStyleSheet("border: none; background-color: transparent;"); // 设置按钮样式
    connect(minimizeButton, &QPushButton::clicked, this, &MainWindow::onMinimizeButtonClicked);

    // 创建关闭按钮
    closeButton = new QPushButton(titleBar);
    closeButton->setGeometry(this->width() - 40, 5, 20, 20); // 设置按钮位置和大小
    closeButton->setIcon(QIcon(":/images/close.png")); // 使用资源文件中的图标
    closeButton->setIconSize(QSize(16, 16)); // 设置图标大小
    closeButton->setStyleSheet("border: none; background-color: transparent;"); // 设置按钮样式
    connect(closeButton, &QPushButton::clicked, this, &MainWindow::onCloseButtonClicked);
}


void MainWindow::onSettingsButtonClicked()
{
    QMessageBox::information(this, "设置", "设置按钮被点击了！");
}

void MainWindow::onMinimizeButtonClicked()
{
    this->showMinimized(); // 最小化窗口
}

void MainWindow::onCloseButtonClicked()
{
    this->close(); // 关闭窗口
}


void MainWindow::initProgress()
{
    // 创建进度条
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100); // 设置进度范围
    progressBar->setValue(0);      // 初始值为 0
				   //
    // 创建状态标签
    statusLabel = new QLabel("准备中...", this);
    statusLabel->setAlignment(Qt::AlignCenter);

    QString output = dbus_utils::tools(dbus_methodStatus);
    if ( output == "error") {
        statusLabel->setText("Error: openFDE Service not started. ");
    	centralLayout->addWidget(statusLabel);
        return ;
    }else  if ( output == "installed\n"){
       btn->show();
       emit imageSignal("/home/warlice/2.png"); // 图片路
       return;
    }

    // 连接 Worker 的信号和槽
    QObject::connect(workThread, &QThread::started, installWorker, &Worker::doInstallWork);
    QObject::connect(installWorker, &Worker::workFinished, workThread, &QThread::quit);
    workThread->start();

    // 创建布局
    currentProgress = 0 ;

    centralWidget->resize(800,100);
    centralWidget->move(0,30);
    centralWidget->show();
    //centralWidget->setStyleSheet("background-color: lightblue;");
    centralWidget->move(0,this->height()/2 - centralWidget->height()/4);

    centralLayout->addWidget(progressBar);

    centralLayout->addWidget(statusLabel);
    extracting=false;
    installing=false;
    downloading=false;

    // 创建定时器
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateProgress);
    timer->start(1000); // 每秒更新一次进度
}



void MainWindow::updateProgress()
{
    QString output = dbus_utils::tools("status");
    QRegularExpression regex("^(\\w+)\\s(\\d+)$");
    QRegularExpressionMatch match = regex.match(output);
    QString action;
    int number;
    if (match.hasMatch()) {
        action = match.captured(1); // 提取状态（如 "downloading" 或 "installing"）
        number = match.captured(2).toInt(); // 提取数字（如 2）
        qDebug() << "状态:" << action;
        qDebug() << "数字:" << number;
    } else {
        qDebug() << "无法解析字符串:" << output;
	    if (output == "installed\n") {
		       timer->stop(); // 进度完成后停止定时器
		       statusLabel->setText("安装完成！");
		       statusLabel->hide();
		       progressBar->hide();
		       btn->show();
		       emit imageSignal("/home/warlice/2.png"); // 图片路
	 }
	    return;
    }
    QString status;
   currentProgress = number;
      if (action ==  "installing") {
	      status = "安装中... ";
		installing=true;
      }else if (action == "extracting") {
	      extracting=true;
		status="解压中... " ;
      }else if (action == "downloading"){
	      downloading = true;
		status="下载中... ";
      }else{
	     if (!installing && !extracting && !downloading){//fix the status reversed by an unexpected status
		      currentProgress = 0 ;
		      status="下载中... ";
	      }
      }
   statusLabel->setText(status + QString::number(currentProgress) + "%");
    // 更新进度条
    progressBar->setValue(currentProgress);
}


