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
    imageLabel->setPixmap(pixmap.scaled(this->size(), Qt::KeepAspectRatioByExpanding));
    imageLabel->setGeometry(0, 30, this->width(), this->height() - 30); // 留出标题栏的空间

    // 创建三角形启动按钮
    startButton = new QPushButton(this);
    startButton->setGeometry(350, 250, 100, 100); // 设置按钮位置和大小

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

    // 连接启动按钮点击事件
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


QHBoxLayout *layout = new QHBoxLayout(titleBar);
    layout->setContentsMargins(5, 0, 5, 0); // 设置布局边距

    // 添加标题
    QLabel *titleLabel = new QLabel("FDE Manager", titleBar); // 创建标题标签
    titleLabel->setStyleSheet("font-size: 14px; font-weight: bold; color: #333333;"); // 设置标题样式
    layout->addWidget(titleLabel); // 将标题添加到布局左侧

    // 添加弹性空间，使按钮靠右
    layout->addStretch();

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
    // 创建并显示安装进度窗口
    installWidget = new InstallWidget(this);
    installWidget->setWindowFlags(Qt::Window | Qt::FramelessWindowHint); // 设置为无边框窗口
    installWidget->move(this->geometry().center() - installWidget->rect().center()); // 居中显示
    installWidget->show();
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
	    if (installWidget) {
		installWidget->raise(); // 将 InstallWidget 提升到最上层
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
    }
}

