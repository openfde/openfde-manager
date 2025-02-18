#include "mainwindow.h"
#include "dbus.h"
#include "logger.h"
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
    this->setFixedSize(420, 267);
    lastShowTime = 0 ;
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
    //imageLabel->setPixmap(pixmap.scaled(this->size(), Qt::KeepAspectRatioByExpanding));
    imageLabel->setGeometry(0, 30, this->width(), this->height() - 30); // 留出标题栏的空间
    imageLabel->setPixmap(pixmap.scaled(425,237, Qt::KeepAspectRatio)); // 缩放图片并显示
    imageLabel->lower();

    btn = new CircleWidgetWithButton(this);
    //btn->move((this->width()-30)/2,(this->height()-30)/2);
    btn->move(100,50);
    btn->show();

    QFile fdeUtils("/usr/bin/fde_utils");
    if (fdeUtils.exists()){
		QProcess *process = new QProcess();
		process->start("bash", QStringList() << "/usr/bin/fde_utils"<<"status");
		process->waitForFinished(-1);
		//获取waitForFinished返回值
		int exitCode = process->exitCode();
		if (exitCode == 1) {
			btn->toggleToStatus(button_start_status);
		}
    }
    // 连接启动按钮点击事件
    connect(this, &MainWindow::imageSignal, this, &MainWindow::showImage);
    connect(btn, &CircleWidgetWithButton::sendMessage, this, &MainWindow::onMessageReceived);
}

static const QString getOpenfdeUrl = "http://phyvirt.openfde.com/getopenfde/get-openfde.sh";

void MainWindow::onMessageReceived( const QString & string , bool withAction) {

	Logger::log(Logger::INFO, QString("ReceiverClass: Received message: %1").arg(string).toStdString());
	installWorker = new Worker();
	if (string  == button_start_status) {
		QString filepath = "/usr/bin/get_fde.sh";
		QFile getfde(filepath);
	//check the exist of the primary script get_fde.sh
		if (!getfde.exists()){
			QUrl url(getOpenfdeUrl); // 下载文件的 URL
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
		int ret = initProgress();
		if ( ret == -1 ){
			Logger::log(Logger::ERROR,"got error for checking whether openfde installed, maybe fde-dbus service not running");
			return ;
		}
		startWorker = new StartWorker();
		startThread = new QThread();
		startWorker->moveToThread(startThread);
		QObject::connect(startThread, &QThread::started, startWorker, &StartWorker::doStartWork);
		QObject::connect(startWorker, &StartWorker::startEnded, startThread, &QThread::quit);
		QObject::connect(startWorker, &StartWorker::startEnded, this, &MainWindow::onRunEnded);	
		startThread->start();
	}else if (string == button_stop_status){
		if ( withAction){
			//执行脚本fde_utils stop 
			QProcess *process = new QProcess();
			process->start("bash", QStringList() << "/usr/bin/fde_utils"<<"stop");
			process->waitForFinished(-1);
			QString output(process->readAllStandardOutput());
			Logger::log(Logger::INFO, QString("fde_utils output: %1").arg(output).toStdString());
			this->showImage(true);
		}
	}
}

void MainWindow::onRunEnded(){
    btn->toggleToStatus(button_stop_status);
}


static const QString fdeUtilsPath = "/usr/bin/fde_utils";
static const QString imagePath = ":/images/openfde.png";
void MainWindow::showImage(bool immediately) {
	QMutex ImageMutex;
    //增加一个记录时间戳的变量
	ImageMutex.lock();
	if (lastShowTime == 0 || (QDateTime::currentMSecsSinceEpoch() - lastShowTime) > 10000 || immediately) {
	lastShowTime = QDateTime::currentMSecsSinceEpoch();
	ImageMutex.unlock();
	} else {
		ImageMutex.unlock();
		Logger::log(Logger::DEBUG,"less than ten secs do not to update screenshot");
		return;
	}
	QFile utilsFile(fdeUtilsPath);
	QString imagePath = imagePath;
	if (utilsFile.exists()) {
		//执行脚本fde_utils status
		QProcess *process = new QProcess();
		process->start("bash", QStringList() << "/usr/bin/fde_utils"<<"status");
		process->waitForFinished(-1);
		//获取waitForFinished返回值
		int exitCode = process->exitCode();
		if (exitCode == 1) {//0 means fde is stopped
			// 加载图片
			Logger::log(Logger::INFO," fde is stared for screenshoting");
			QString retScreen = dbus_utils::utils("screenshot");
			if (retScreen == dbus_errorS) {
				imageLabel->setText("Error: openFDE Service not started. ");
				return ;
			}
			imagePath = "/tmp/openfde_screen.jpg";
			QFile screenFile(imagePath);
			if (! screenFile.exists()) {
				imagePath = ":/images/openfde.png";
			}
		}
	}
   
	QPixmap pixmap(imagePath);
	if (pixmap.isNull()) {
		Logger::log(Logger::ERROR,QString(" failed to load %1 for pixmap").arg(imagePath).toStdString());
		imageLabel->setText("Failed to load image!"); // 如果图片加载失败，显示错误信息
	} else {
		Logger::log(Logger::INFO," show screenshot");
		centralWidget->resize(this->width(),this->height()-30);
		centralWidget->move(0,30);
		imageLabel->setGeometry(0, 30, this->width(), this->height() - 30); // 留出标题栏的空间
		//imageLabel->setPixmap(pixmap.scaled(this->size(), Qt::KeepAspectRatioByExpanding));
		imageLabel->setPixmap(pixmap.scaled(425,237, Qt::KeepAspectRatio)); // 缩放图片并显示
		imageLabel->show();
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
   // titleBar->setStyleSheet("background-color: #1e90ff;"); // 设置标题栏背景颜色为白色
    titleBar->setStyleSheet("background-color: #e0e0e0;"); // 设置标题栏背景颜色为白色
							   //
    QHBoxLayout *layout = new QHBoxLayout(titleBar);
    layout->setContentsMargins(5, 0, 5, 0); // 设置布局边距

    // 添加标题
    QLabel *titleLabel = new QLabel("OpenFDE", titleBar); // 创建标题标签
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333333;"); // 设置标题样式
    //titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #808080;"); // 设置标题样式
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
    //获取版本信息
    QString versionFDE = dbus_utils::tools("version_fde");
    QString ctrlFDE = dbus_utils::tools("version_ctrl");
    
    //创建无图标的消息框
    QMessageBox msgBox(this);
    msgBox.setFixedSize(100,50);
    msgBox.setWindowTitle("版本信息");
    msgBox.setText("FDE版本：" + versionFDE + "\n控制程序版本：" + ctrlFDE);
    msgBox.setIcon(QMessageBox::NoIcon);
    msgBox.exec();
}

void MainWindow::onMinimizeButtonClicked()
{
    this->showMinimized(); // 最小化窗口
}

void MainWindow::onCloseButtonClicked()
{
    this->close(); // 关闭窗口
}


int MainWindow::initProgress()
{
    QString output = dbus_utils::tools(dbus_methodStatus);
    if ( output == "error") {
        return -1;
    }else  if ( output == "installed\n"){
       emit imageSignal(); 
       return 0;
    }
    imageLabel->hide();
    btn->hide();

    workThread = new QThread();
    // 将 Worker 移动到子线程
    installWorker->moveToThread(workThread);

    // 创建进度条
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100); // 设置进度范围
    progressBar->setValue(0);      // 初始值为 0
				   //
    // 创建状态标签
    statusLabel = new QLabel("准备中...", this);
    statusLabel->setAlignment(Qt::AlignCenter);

    // 连接 Worker 的信号和槽
    QObject::connect(workThread, &QThread::started, installWorker, &Worker::doInstallWork);
    QObject::connect(installWorker, &Worker::workFinished, workThread, &QThread::quit);
    workThread->start();

    // 创建布局
    currentProgress = 0 ;

    centralWidget->resize(600,100);
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
    return 0;
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
		Logger::log(Logger::INFO,QString(" status %1 number %2").arg(action).arg(number).toStdString()); 
	} else {
        	Logger::log( Logger::DEBUG,"No matching status found: " + output.toStdString());
		if (output == "installed\n") {
			timer->stop(); // 进度完成后停止定时器
        		Logger::log( Logger::DEBUG,"installed successfully");
			statusLabel->setText("安装完成！");
			statusLabel->hide();
			progressBar->hide();
			btn->show();
			emit imageSignal(); 
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


