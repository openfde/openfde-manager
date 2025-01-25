#include "mainwindow.h"
#include <QPixmap>
#include <QPainter>
#include <QPolygon>
#include <QTimer>
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

    // Add a new QWidget for the circular background
QWidget *circularBackground = new QWidget(this);
circularBackground->setStyleSheet("background-color: gray; border-radius: 60px;");
//circularBackground->lower(); // Ensure it's below the button in z-order

    // 创建QLabel用于显示图片
    // 创建三角形启动按钮
    startButton = new QPushButton(this);
    //centralLayout->addWidget(startButton);
    startButton->setGeometry(350, 250, 100, 100); // 设置按钮位置和大小
circularBackground->setGeometry(startButton->x() - 30, startButton->y() - 30, 120, 120); // Position and size the circle

    // 设置按钮形状为三角形
    QPolygon polygon;
    polygon <<  QPoint(80, 50) << QPoint(0, 0) << QPoint(0,100); // 顶点在右侧
    QRegion region(polygon);
    startButton->setMask(region);

    // 设置按钮样式
    startButton->setIcon(QIcon(":/images/start.png"));
    startButton->setIconSize(QSize(60, 60)); // 设置图标大小
    //startButton->setStyleSheet("border: none; background-color: transparent;"); // 设置按钮样式
    startButton->setStyleSheet("background-color: green;");
    startButton->raise();


    // 连接启动按钮点击事件
    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartButtonClicked);
    connect(this, &MainWindow::imageSignal, this, &MainWindow::showImage);
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

void MainWindow::onStartButtonClicked()
{
    startButton->close();
    imageLabel->hide();
    initProgress();
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

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragPosition = event->globalPos() - frameGeometry().topLeft();
        event->accept();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        move(event->globalPos() - dragPosition);
        event->accept();
    }
}


void clearWidgetChildren(QWidget *widget) {
    if (!widget) return;

    // 查找所有子部件
    QList<QWidget *> children = widget->findChildren<QWidget *>();

    // 遍历并删除子部件
    for (QWidget *child : children) {
        child->setParent(nullptr); // 从父部件中移除
        delete child;              // 删除部件
    }
}

void MainWindow::initProgress()
{
    // 创建布局
    this->downloading = false;
    this->extracting = false;
    this->installing = false;
    
    centralWidget->resize(800,100);
    centralWidget->move(0,30);
    centralWidget->show();
    //centralWidget->setStyleSheet("background-color: lightblue;");
    centralWidget->move(0,this->height()/2 - centralWidget->height()/4);

    // 创建进度条
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100); // 设置进度范围
    progressBar->setValue(0);      // 初始值为 0
    centralLayout->addWidget(progressBar);

    // 创建状态标签
    statusLabel = new QLabel("准备中...", this);
    statusLabel->setAlignment(Qt::AlignCenter);
    centralLayout->addWidget(statusLabel);

    // 创建定时器
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateProgress);
    timer->start(1000); // 每秒更新一次进度
}

void MainWindow::updateProgress()
{
    // 模拟安装进度
    currentProgress += 60; // 每次增加 10%
    if (currentProgress > 100) {
       if (this->recyleNum == 1 ){
               downloading = true;
       }
       if (this->recyleNum == 2 ){
               extracting = true;
       }
       if (this->recyleNum == 3 ){
               installing = true;
       }else {
               this->recyleNum++;
               currentProgress = 0 ;
       }
    }
    // 更新进度条
    progressBar->setValue(currentProgress);

    if (!downloading && !extracting && !installing) {
        statusLabel->setText("下载中... " + QString::number(currentProgress) + "%");
    }else if (downloading && !extracting && !installing) {
        statusLabel->setText("解压中... " + QString::number(currentProgress) + "%");
    }else if (downloading && extracting && !installing) {
        statusLabel->setText("安装中... " + QString::number(currentProgress) + "%");
    } else {
       timer->stop(); // 进度完成后停止定时器
       statusLabel->setText("安装完成！");
       //clearWidgetChildren(centralWidget);
       //centralWidget->close();
       qDebug()<<"finish";
       statusLabel->hide();
       progressBar->hide();
       emit imageSignal("/home/warlice/2.png"); // 图片路
    }
}

