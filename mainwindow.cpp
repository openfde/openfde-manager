#include "mainwindow.h"
#include <QPixmap>
#include <QPainter>
#include <QPolygon>
#include <QMessageBox>
#include <QIcon>
#include <QHBoxLayout>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    // 移除默认的标题栏
    this->setWindowFlags(Qt::FramelessWindowHint);

    // 设置窗口大小
    this->setFixedSize(800, 600);

    // 创建自定义标题栏
    createTitleBar();

    // 创建QLabel用于显示图片
    imageLabel = new QLabel(this);
    QPixmap pixmap(":/images/background.jpg"); // 图片路径，需要将图片添加到资源文件中
    QPixmap pixmap(":/images/background.jpg");
    imageLabel->setPixmap(pixmap.scaled(this->size(), Qt::KeepAspectRatioByExpanding));
    imageLabel->setGeometry(0, 30, this->width(), this->height() - 30); // 留出标题栏的空间
    imageLabel->setGeometry(0, 30, this->width(), this->height() - 30);

    // 创建三角形启动按钮
    startButton = new QPushButton(this);
    startButton->setGeometry(350, 250, 100, 100); // 设置按钮位置和大小
    startButton->setGeometry(350, 250, 100, 100);

    // 设置按钮形状为三角形
    QPolygon polygon;
     polygon <<  QPoint(80, 50) << QPoint(0, 0) << QPoint(0,100); // 顶点在右侧
    polygon << QPoint(80, 50) << QPoint(0, 0) << QPoint(0, 100);
    QRegion region(polygon);
    startButton->setMask(region);

    // 设置按钮样式
    startButton->setIcon(QIcon(":/images/start.png"));
    startButton->setIconSize(QSize(60, 60)); // 设置图标大小
    //startButton->setStyleSheet("border: none; background-color: transparent;"); // 设置按钮样式
    startButton->setIconSize(QSize(60, 60));
    startButton->setStyleSheet("background-color: green;");

    // 连接启动按钮点击事件
    startButton->setIconSize(QSize(60, 60));
    startButton->setStyleSheet("background-color: green;");
    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartButtonClicked);
}

MainWindow::~MainWindow()
{
}

void MainWindow::createTitleBar()
{
    // 创建标题栏容器
    QWidget *titleBar = new QWidget(this);
    installWidget = 0;
    titleBar->setGeometry(0, 0, this->width(), 30); // 设置标题栏大小
    //titleBar->setStyleSheet("background-color: #2c3e50;"); // 设置标题栏背景颜色
    titleBar->setStyleSheet("background-color: #ffffff;"); // 设置标题栏背景颜色为白色


    titleBar->setGeometry(0, 0, this->width(), 30);
    titleBar->setStyleSheet("background-color: #ffffff;");
    titleBar->setGeometry(0, 0, this->width(), 30);
    titleBar->setStyleSheet("background-color: #ffffff;");
    QHBoxLayout *layout = new QHBoxLayout(titleBar);
    layout->setContentsMargins(5, 0, 5, 0); // 设置布局边距
    layout->setContentsMargins(5, 0, 5, 0);

    // 添加标题
    QLabel *titleLabel = new QLabel("FDE Manager", titleBar); // 创建标题标签
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333333;"); // 设置标题样式
    layout->addWidget(titleLabel); // 将标题添加到布局左侧
    QLabel *titleLabel = new QLabel("FDE Manager", titleBar);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333333;");
    layout->addWidget(titleLabel);

    // 添加弹性空间，使按钮靠右
    QLabel *titleLabel = new QLabel("FDE Manager", titleBar);
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333333;");
    layout->addWidget(titleLabel);
    layout->addStretch();

    // 创建设置按钮
    settingsButton = new QPushButton(titleBar);
    settingsButton->setGeometry(this->width() - 120, 5, 20, 20); // 设置按钮位置和大小
    settingsButton->setIcon(QIcon(":/images/settings.png")); // 使用资源文件中的图标
    settingsButton->setIconSize(QSize(16, 16)); // 设置图标大小
    settingsButton->setStyleSheet("border: none; background-color: transparent;"); // 设置按钮样式
    settingsButton->setGeometry(this->width() - 120, 5, 20, 20);
    settingsButton->setIcon(QIcon(":/images/settings.png"));
    settingsButton->setIconSize(QSize(16, 16));
    settingsButton->setStyleSheet("border: none; background-color: transparent;");
    settingsButton->setGeometry(this->width() - 120, 5, 20, 20);
    settingsButton->setIcon(QIcon(":/images/settings.png"));
    settingsButton->setIconSize(QSize(16, 16));
    settingsButton->setStyleSheet("border: none; background-color: transparent;");
    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsButtonClicked);

    // 创建最小化按钮
    minimizeButton = new QPushButton(titleBar);
    minimizeButton->setGeometry(this->width() - 80, 5, 20, 20); // 设置按钮位置和大小
    minimizeButton->setIcon(QIcon(":/images/minimize.png")); // 使用资源文件中的图标
    minimizeButton->setIconSize(QSize(16, 16)); // 设置图标大小
    minimizeButton->setStyleSheet("border: none; background-color: transparent;"); // 设置按钮样式
    minimizeButton->setGeometry(this->width() - 80, 5, 20, 20);
    minimizeButton->setIcon(QIcon(":/images/minimize.png"));
    minimizeButton->setIconSize(QSize(16, 16));
    minimizeButton->setStyleSheet("border: none; background-color: transparent;");
    minimizeButton->setGeometry(this->width() - 80, 5, 20, 20);
    minimizeButton->setIcon(QIcon(":/images/minimize.png"));
    minimizeButton->setIconSize(QSize(16, 16));
    minimizeButton->setStyleSheet("border: none; background-color: transparent;");
    connect(minimizeButton, &QPushButton::clicked, this, &MainWindow::onMinimizeButtonClicked);

    // 创建关闭按钮
    closeButton = new QPushButton(titleBar);
    closeButton->setGeometry(this->width() - 40, 5, 20, 20); // 设置按钮位置和大小
    closeButton->setIcon(QIcon(":/images/close.png")); // 使用资源文件中的图标
    closeButton->setIconSize(QSize(16, 16)); // 设置图标大小
    closeButton->setStyleSheet("border: none; background-color: transparent;"); // 设置按钮样式
    closeButton->setGeometry(this->width() - 40, 5, 20, 20);
    closeButton->setIcon(QIcon(":/images/close.png"));
    closeButton->setIconSize(QSize(16, 16));
    closeButton->setStyleSheet("border: none; background-color: transparent;");
    closeButton->setGeometry(this->width() - 40, 5, 20, 20);
    closeButton->setIcon(QIcon(":/images/close.png"));
    closeButton->setIconSize(QSize(16, 16));
    closeButton->setStyleSheet("border: none; background-color: transparent;");
    connect(closeButton, &QPushButton::clicked, this, &MainWindow::onCloseButtonClicked);
}

void MainWindow::onStartButtonClicked()
{
    startButton->close();
    initProgress();
}

void MainWindow::onSettingsButtonClicked()
{
    QMessageBox::information(this, "设置", "设置按钮被点击了！");
}

void MainWindow::onMinimizeButtonClicked()
{
    this->showMinimized(); // 最小化窗口
    this->showMinimized();
}

void MainWindow::onCloseButtonClicked()
{
    this->close(); // 关闭窗口
    this->close();
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
	    if (installWidget) {
		installWidget->raise(); // 将 InstallWidget 提升到最上层
            installWidget->raise();
		 }
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}

void MainWindow::moveEvent(QMoveEvent *event)
{
    QMainWindow::moveEvent(event);
    // 如果 InstallWidget 存在，更新其位置
    if (installWidget) {
        installWidget->move(this->geometry().center() - installWidget->rect().center());
	installWidget->raise(); // 将 InstallWidget 提升到最上层
        installWidget->raise();
    }
}


void MainWindow::initProgress()
{
    this->download=false;
    this->extra=false;
    this->install=false;
    this->download = false;
    this->extra = false;
    this->install = false;

    // 创建布局
    this->download = false;
    this->extra = false;
    this->install = false;
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->move(this->geometry().center());
    

    // 创建进度条
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100); // 设置进度范围
    progressBar->setValue(0);      // 初始值为 0
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    layout->addWidget(progressBar);

    // 创建状态标签
    statusLabel = new QLabel("准备中...", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(statusLabel);

    // 创建定时器
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateProgress);
    timer->start(1000); // 每秒更新一次进度
    timer->start(1000);
}

void MainWindow::updateProgress()
{
    // 模拟安装进度
    currentProgress += 10; // 每次增加 10%
    currentProgress += 10;
    if (currentProgress > 100) {
	if (this->recyleNum == 0 ){
        if (this->recyleNum == 0) {
		download = true;
	}
	if (this->recyleNum == 1 ){
        if (this->recyleNum == 1) {
		extra = true;
	}
	if (this->recyleNum == 2 ){
        if (this->recyleNum == 2) {
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
            this->recyleNum++;
            currentProgress = 0;
    }
}

    progressBar->setValue(currentProgress);

    if (!download && !extra && !install) {
        statusLabel->setText("下载中... " + QString::number(currentProgress) + "%");
    } else if (download && !extra && !install) {
        statusLabel->setText("解压中... " + QString::number(currentProgress) + "%");
    } else if (download && extra && !install) {
        statusLabel->setText("安装中... " + QString::number(currentProgress) + "%");
    } else {
        timer->stop();
        statusLabel->setText("安装完成！");
        this->close();
        this->deleteLater();
    }
}