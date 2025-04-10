// Copyright (C) 2025 OpenFDE.
// SPDX-License-Identifier: Apache-2.0

#include "mainwindow.h"
#include "dbus.h"
#include <QDir>
#include <QCheckBox>
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
#include <QFile>
#include <QTextStream>
#include <QFutureWatcher>
#include <QFuture>
#include <QMovie>
#include <QtConcurrent>
#include "shellUtils.h"

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

    btn = new ShapeButton(this);
    //btn->move((this->width()-30)/2,(this->height()-30)/2);
    btn->move(100,50);
    btn->show();

    if ( ! shellUtils::isOpenfdeClosed() ){
        btn->toggleToStatus(button_start_status) ;
    }
    // 连接启动按钮点击事件
    connect(this, &MainWindow::imageSignal, this, &MainWindow::showImage);
    connect(btn, &ShapeButton::sendMessage, this, &MainWindow::onMessageReceived,Qt::QueuedConnection);
    //must use queuedConnection because failed start will toggle the button to stop, if use blockConnection, the shape of the button will not update correctly.
    connect(this,&MainWindow::sendStatusUpdateMessage, btn,&ShapeButton::receiveStatusUpdateMessage,Qt::QueuedConnection);
}

static const QString getOpenfdeUrl = "https://openfde.com/getopenfde/ex-install-openfde.sh";

void MainWindow::onMessageReceived( const QString & string , bool withAction) {

	Logger::log(Logger::INFO, QString("ReceiverClass: Received message: %1").arg(string).toStdString());
	QString securityStatus = dbus_utils::security(dbus_utils::methodSecurityQuery);
	Logger::log(Logger::INFO, QString("query security status: %1").arg(securityStatus).toStdString());
	if (securityStatus == dbus_utils::statusSecurityEnable) {
		QMessageBox::StandardButton reply = QMessageBox::question(this, 
			tr("确认操作"), 
			tr("确认是否关闭应用执行控制"),
			QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes) {
			QString result = dbus_utils::security(dbus_utils::methodSecurityDisable);
			if (result == dbus_utils::errorS) {
				QMessageBox::critical(this, tr("Error"), tr("关闭系统安全设置失败"), QMessageBox::Ok);
				sendStatusUpdateMessage(button_stop_status);
				return ;
			}
		}else{
			sendStatusUpdateMessage(button_stop_status);
			return ;
		}
	}
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
				QMessageBox::critical(this,tr("Error"), tr("网络或磁盘故障"),QMessageBox::Ok);
				sendStatusUpdateMessage(button_stop_status);
				return ;
			}
			//construct /usr/bin/get_fde.sh
			QString retS = dbus_utils::construct();
			if (retS == dbus_utils::errorS) {
				// 创建状态标签
				QMessageBox::critical(this,tr("Error"), tr("FDE Dbus服务未启动"),QMessageBox::Ok);
				sendStatusUpdateMessage(button_stop_status);
				return ;
			}
		}
		int ret = initProgress();
		if ( ret == -1 ){
			QMessageBox::critical(this,tr("Error"), tr("FDE Dbus服务未启动"),QMessageBox::Ok);
			Logger::log(Logger::ERROR,"got error for checking whether openfde installed, maybe fde-dbus service not running");
			return ;
		}
		if (ret == 2 ) {//installing
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
			Logger::log(Logger::INFO, " on mesage for button stop status fde_utils ");
			shellUtils::stopOpenfde();
			this->showImage(true);
		}
	}
}

void MainWindow::onRunEnded(){
    btn->toggleToStatus(button_stop_status);
}


static const QString imagesPath = ":/images/openfde.png";
void MainWindow::showImage(bool immediately) {
	QMutex ImageMutex;
	if ( preparing ){
		return ;
	}
	ImageMutex.lock();
    	//增加一个记录时间戳的变量
	if (lastShowTime == 0 || (QDateTime::currentMSecsSinceEpoch() - lastShowTime) > 10000 || immediately) {
		lastShowTime = QDateTime::currentMSecsSinceEpoch();
		ImageMutex.unlock();
	} else {
		ImageMutex.unlock();
		Logger::log(Logger::DEBUG,"less than ten secs do not to update screenshot");
		return;
	}
	QString imagePath = imagesPath;
	if (! shellUtils::isOpenfdeClosed()){
		// 加载图片
		Logger::log(Logger::INFO," fde is started for screenshoting");
		QString retScreen = dbus_utils::utils("screenshot");
		if (retScreen == dbus_utils::errorS) {
			//QMessageBox::critical(this,tr("Error"), tr("FDE Dbus服务未启动"),QMessageBox::Ok);
			return ;
		}
		imagePath = "/tmp/openfde_screen.jpg";
		QFile screenFile(imagePath);
		if (! screenFile.exists()) {
			imagePath = ":/images/openfde.png";
		}
	}
   
	QPixmap pixmap(imagePath);
	if (pixmap.isNull()) {
		Logger::log(Logger::ERROR,QString(" failed to load %1 for pixmap").arg(imagePath).toStdString());
		//imageLabel->setText("Failed to load image!"); // 如果图片加载失败，显示错误信息
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
	layout->setSpacing(5);

	layout->setAlignment(Qt::AlignLeft); // Align widgets to the left
    QLabel *iconLabel = new QLabel(titleBar);
    QPixmap icon(":/images/openfde_icon.png");
    iconLabel->setPixmap(icon.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation));
	layout->addWidget(iconLabel);
    // 添加标题
    QLabel *titleLabel = new QLabel("OpenFDE管理中心", titleBar); // 创建标题标签
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333333;"); // 设置标题样式
    //titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #808080;"); // 设置标题样式
    layout->addWidget(titleLabel); // 将标题添加到布局左侧

    // 创建设置按钮
    settingsButton = new QPushButton(titleBar);
    settingsButton->setFocusPolicy(Qt::NoFocus);
    settingsButton->setGeometry(this->width() - 120, 5, 20, 20); // 设置按钮位置和大小
    settingsButton->setIcon(QIcon(":/images/settings.png")); // 使用资源文件中的图标
    settingsButton->setIconSize(QSize(20, 20)); // 设置图标
    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsButtonClicked);
    settingsButton->setStyleSheet(
		"QPushButton {"
		"    border: none;"
		"    background-color: transparent;"
		"}"
		"QPushButton:hover {"
		"    background-color: rgba(0, 0, 0, 0.1);"
		"}"
		"QPushButton:pressed {"
		"    background-color: rgba(0, 0, 0, 0.2);"
		"}"
	);

    // 创建最小化按钮
    minimizeButton = new QPushButton(titleBar);
    minimizeButton->setGeometry(this->width() - 80, 5, 20, 20); // 设置按钮位置和大小
    minimizeButton->setIcon(QIcon(":/images/minimize.png")); // 使用资源文件中的图标
    minimizeButton->setIconSize(QSize(20,20)); // 设置图标大小
    minimizeButton->setFocusPolicy(Qt::NoFocus);
    minimizeButton->setStyleSheet(
		"QPushButton {"
		"    border: none;"
		"    background-color: transparent;"
		"}"
		"QPushButton:hover {"
		"    background-color: rgba(0, 0, 0, 0.1);"
		"}"
		"QPushButton:pressed {"
		"    background-color: rgba(0, 0, 0, 0.2);"
		"}"
	);
    connect(minimizeButton, &QPushButton::clicked, this, &MainWindow::onMinimizeButtonClicked);

    // 创建关闭按钮
    closeButton = new QPushButton(titleBar);
    closeButton->setFocusPolicy(Qt::NoFocus);
    closeButton->setGeometry(this->width() - 40, 5, 20, 20); // 设置按钮位置和大小
    closeButton->setIcon(QIcon(":/images/close.png")); // 使用资源文件中的图标
    closeButton->setIconSize(QSize(20,20)); // 设置图标大小
    closeButton->setStyleSheet(
		"QPushButton {"
		"    border: none;"
		"    background-color: transparent;"
		"}"
		"QPushButton:hover {"
		"    background-color: rgba(0, 0, 0, 0.1);"
		"}"
		"QPushButton:pressed {"
		"    background-color: rgba(0, 0, 0, 0.2);"
		"}"
	);
    connect(closeButton, &QPushButton::clicked, this, &MainWindow::onCloseButtonClicked);
}

void MainWindow::onInstallWorkerFinishedError(QString err)
{
	Logger::log(Logger::DEBUG,"on install worker finished error" + err.toStdString());
	ceaseInstalling();
	QMessageBox::critical(this, "Error",tr(dbus_utils::parseError(err)),  QMessageBox::Ok);
	btn->toggleToStatus(button_stop_status);
}
		

void MainWindow::onSettingsButtonClicked()
{
    //获取版本信息
	QString versionFDE, ctrlFDE;
	QFile getfde("/usr/bin/get_fde.sh");
	//check the exist of the primary script get_fde.sh
	if (!getfde.exists()){
		versionFDE = tr("未安装");
		ctrlFDE = tr("未安装");
	}else {
		versionFDE = dbus_utils::tools(dbus_utils::methodStatus);
		if (versionFDE != dbus_utils::openfdeStatusInstalled) 
			versionFDE = tr("未安装");
		else{
			versionFDE = dbus_utils::tools(dbus_utils::methodVersionFDE);
		}
		ctrlFDE = dbus_utils::tools(dbus_utils::methodVersionCtrl);
		if (ctrlFDE.contains(dbus_utils::errorS)) {
			ctrlFDE = tr("未安装");
		}
	}
	// Filter out newline characters from version strings
	versionFDE.replace("\n", "");
	ctrlFDE.replace("\n", "");
	// Create widget for version info
	
	QDialog *versionWidget = new QDialog(this);
	//QWidget *versionWidget = new QWidget();
	versionWidget->setFixedSize(300, 150);
	versionWidget->setWindowIcon(QIcon(":/images/settings.png"));

	QVBoxLayout *vLayout = new QVBoxLayout(versionWidget);
	versionWidget->setWindowTitle(tr("系统设置"));

	//versionWidget->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
	versionWidget->setModal(true);
	// Center the dialog relative to parent window
	versionWidget->move(
		this->geometry().center().x() - versionWidget->width()/2,
		this->geometry().center().y() - versionWidget->height()/2
	);

	// FDE Version
	QLabel *fdeLabel = new QLabel(tr("OpenFDE版本")+":", versionWidget);
	QLabel *fdeValue = new QLabel(versionFDE, versionWidget);
	QHBoxLayout *fdeLayout = new QHBoxLayout();
	fdeLayout->addWidget(fdeLabel);
	fdeLayout->addWidget(fdeValue);
	fdeLayout->setAlignment(Qt::AlignCenter); // Align widgets to the left
	fdeLayout->setSpacing(10); // Add some spacing between widgets

	// Control Version
	QLabel *ctrlLabel = new QLabel(tr("控制程序版本")+":",versionWidget);
	QLabel *ctrlValue = new QLabel(ctrlFDE,versionWidget);
	QHBoxLayout *ctrlLayout = new QHBoxLayout();
	ctrlLayout->addWidget(ctrlLabel);
	ctrlLayout->addWidget(ctrlValue);
	ctrlLayout->setAlignment(Qt::AlignCenter); // Align widgets to the left
	ctrlLayout->setSpacing(10); // Add some spacing between widgets

	// Update button
	QCheckBox *deleteDataCheckbox = new QCheckBox(tr("卸载时一起删除数据"), versionWidget);
    	deleteDataCheckbox->setFocusPolicy(Qt::NoFocus);
	QPushButton *uninstallBtn = new QPushButton(tr("卸载"), versionWidget);
	uninstallBtn->setFocusPolicy(Qt::NoFocus);
	QHBoxLayout *uninstallLayout = new QHBoxLayout();
	uninstallLayout->addWidget(deleteDataCheckbox);
	uninstallLayout->addWidget(uninstallBtn);

	connect(uninstallBtn, &QPushButton::clicked, this,[this, deleteDataCheckbox]()  {
		QString ret = dbus_utils::tools(dbus_utils::methodStatus);
		if ( ret != dbus_utils::openfdeStatusInstalled ) {
			QMessageBox* msgBox = new QMessageBox(this);
			msgBox->setIcon(QMessageBox::Warning);
			msgBox->setText(tr("OpenFDE未安装"));
			msgBox->setWindowTitle(tr("OpenFDE未安装"));
			msgBox->show();
			return;
		}
		QMessageBox::StandardButton reply = QMessageBox::question(this,
                       tr("确认卸载"),
                       tr("确定要卸载OpenFDE吗？"),
                       QMessageBox::Yes | QMessageBox::No);
		if (reply == QMessageBox::Yes) {
			if ( ! shellUtils::isOpenfdeClosed() ){
				QMessageBox::critical(this, tr("Error"), tr("请先关闭OpenFDE再卸载"), QMessageBox::Ok);
				return;
		   	}
			
			// Start loading animation
			 QDialog *animationWidget = new QDialog(this);

			animationWidget->setFixedSize(100, 100);

			animationWidget->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
			animationWidget->setWindowModality(Qt::ApplicationModal);

			QMovie *loadingAnimation = new QMovie(":/images/loading.gif");
			QLabel *loadingLabel = new QLabel(animationWidget);
			loadingLabel->setMovie(loadingAnimation);
			loadingAnimation->setScaledSize(QSize(50,50));
			loadingLabel->setAlignment(Qt::AlignCenter);
			loadingLabel->setGeometry(animationWidget->geometry().center().x() - 50, animationWidget->geometry().center().y() - 50, 100, 100);
			animationWidget->show();
			loadingLabel->show();
			loadingAnimation->start();

			// Perform the clear operation asynchronously
			QFuture<void> future = QtConcurrent::run([deleteDataCheckbox,this,animationWidget ]() {
				QString output = dbus_utils::tools(dbus_utils::methodUninstall);
				if (output.contains(dbus_utils::errorS)) {
					QMetaObject::invokeMethod(this, [this,output]() {
						QMessageBox::critical(this, tr("Error"), tr(dbus_utils::parseError(output)), QMessageBox::Ok);
					}, Qt::QueuedConnection);
					return;
				}
				bool deleteData = deleteDataCheckbox->isChecked();
				if (deleteData) {
					QString homeDir = QDir::homePath();
					dbus_utils::clear(homeDir);
				}
				QMetaObject::invokeMethod(this, [this]() {
					QMessageBox* msgBox = new QMessageBox(this);
					msgBox->setIcon(QMessageBox::Information);
					msgBox->setText(tr("卸载成功"));
					msgBox->setWindowTitle(tr("卸载成功"));
					msgBox->show();
				}, Qt::QueuedConnection);
			});

			// Cancel the animation after the operation is complete
			QFutureWatcher<void> *watcher = new QFutureWatcher<void>(this);
			connect(watcher, &QFutureWatcher<void>::finished, this, [animationWidget,loadingLabel, loadingAnimation, watcher]() {
				loadingAnimation->stop();
				loadingLabel->hide();
				animationWidget->close();
				animationWidget->deleteLater();
				loadingLabel->deleteLater();
				watcher->deleteLater();
			});
			watcher->setFuture(future);
		}
	});

	vLayout->addLayout(fdeLayout);
	vLayout->addLayout(ctrlLayout);
	vLayout->addLayout(uninstallLayout);
	versionWidget->setLayout(vLayout);
	versionWidget->show();
}

void MainWindow::onMinimizeButtonClicked()
{
    this->hide();
}

void MainWindow::onCloseButtonClicked()
{
    this->close(); // 关闭窗口
}


int MainWindow::initProgress()
{
    QString output = dbus_utils::tools(dbus_utils::methodStatus);
    if ( output == "error") {
        return -1;
    }else  if ( output == "installed\n"){
       emit imageSignal(); 
       return 0;
    }
    imageLabel->hide();
    btn->hide();

    preparing=true;
    workThread = new QThread();
    // 将 Worker 移动到子线程
    installWorker->moveToThread(workThread);
    // 创建状态标签
    statusLabel = new QLabel(tr("准备中..."), this);
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet(
		"QLabel {"
		"    font-size: 12px;"
		"    font-weight: bold;"
		"    color: #1E90FF;"  
		"}"
	);

    // 创建进度条
    progressBar = new QProgressBar(this);
    progressBar->setStyleSheet(
		"QProgressBar {"
		"   border: 2px solid #1E90FF;"
		"   border-radius: 5px;"
		"   background-color: #F5F5F5;"
		"   color: black;"
		"   text-align: center;"
		"}"
		"QProgressBar::chunk {"
		"   background-color: #1E90FF;"
		"   border-radius: 3px;"
		"}"
	);
    progressBar->setRange(0, 100); // 设置进度范围
    progressBar->setValue(0);      // 初始值为 0
    progressBar->setTextVisible(false);
// 创建取消按钮
    cancelButton = new QPushButton(tr("取消"), this);
    cancelButton->setStyleSheet(
		"QPushButton {"
		"   background-color: #E0E0E0;"
		"   border: 2px solid #CCCCCC;"
		"   border-radius: 5px;"
		"   padding: 5px 10px;"
		"}"
		"QPushButton:hover {"
		"   background-color: #D0D0D0;"
		"}"
	);
	
    connect(cancelButton, &QPushButton::clicked, this, &MainWindow::cancelInstalling);
    //statusLabel->move(progressBar->x(), progressBar->y() - 25);
    cancelButton->setParent(this);
    cancelButton->move(300,200);
    cancelButton->show();
    statusLabel->move(160,100);
    statusLabel->show();

    // 连接 Worker 的信号和槽
    QObject::connect(workThread, &QThread::started, installWorker, &Worker::doInstallWork);
    QObject::connect(installWorker, &Worker::workFinished, workThread, &QThread::quit);
    QObject::connect(installWorker, &Worker::workFinishedErr, this, &MainWindow::onInstallWorkerFinishedError);
    workThread->start();

    // 创建布局
    currentProgress = 0 ;

    //centralLayout->addWidget(statusLabel);
    centralLayout->addWidget(progressBar);

    centralWidget->resize(420,100);
    //centralWidget->move(0,-30);
    centralWidget->show();
    //centralWidget->setStyleSheet("background-color: lightblue;");
    centralWidget->move(0,this->height()/2 - centralWidget->height()/4);


    extracting=false;
    installing=false;
    downloading=false;

    // 创建定时器
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateProgress);
    timer->start(1000); // 每秒更新一次进度
    return 2;
}

void MainWindow::cancelInstalling(){
	QMessageBox::StandardButton reply = QMessageBox::question(this, 
	"确认取消", 
	tr("确定要取消下载吗?"),
	QMessageBox::Yes | QMessageBox::No);
	if (reply == QMessageBox::Yes) {
		// 执行取消脚本
		QString output = dbus_utils::tools(dbus_utils::methodStop);
		//progress->close();
		// 清理界面
		ceaseInstalling();
		btn->toggleToStatus(button_stop_status);
	}
}


void MainWindow::ceaseInstalling() {
	cancelButton->close();
	statusLabel->hide();
	progressBar->hide();
	installing = false;
	downloading = false;
	extracting = false;
	preparing = false;
	timer->stop(); // 进度完成后停止定时器
	btn->show();
	emit imageSignal(true); 
	Logger::log(Logger::INFO,"cease installing");
}

void MainWindow::updateProgress()
{
	QString output = dbus_utils::tools(dbus_utils::methodStatus);
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
        		Logger::log( Logger::DEBUG,"installed successfully");
			statusLabel->setText(tr("安装完成!"));
			currentProgress=100;
			progressBar->setValue(currentProgress);
			ceaseInstalling();
    			btn->toggleToStatus(button_start_status,true);
		}
	 	return;
	}
	QString status;
	currentProgress = number;
	if (action ==  "installing") {
		status = tr("安装中...");
		installing=true;
	}else if (action == "extracting") {
		extracting=true;
		status=tr("解压中...");
	}else if (action == "downloading"){
		downloading = true;
		status=tr("下载中...");
	}else{
		if (!installing && !extracting && !downloading){//fix the status reversed by an unexpected status
			currentProgress = 0 ;
			status=tr("下载中...");
		}
	}
	statusLabel->setText(status);
	// 更新进度条
	progressBar->setValue(currentProgress);
}


