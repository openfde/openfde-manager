#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>
#include <QPixmap>
#include <QDebug>
#include <QPainter>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    //void moveEvent(QMoveEvent *event) override; // 重写 moveEvent 函数
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);

        QPainter painter(this);

        // 设置抗锯齿
        painter.setRenderHint(QPainter::Antialiasing);

        // 设置圆的背景色为淡绿色
        painter.setBrush(QColor(144, 238, 144)); // 淡绿色
        painter.setPen(Qt::NoPen);

        // 计算圆的位置（窗口中央）
        int circleDiameter = 60;
        int circleX = (width() - circleDiameter) / 2;
        int circleY = (height() - circleDiameter) / 2;

        // 绘制圆
        painter.drawEllipse(circleX, circleY, circleDiameter, circleDiameter);
    }

private slots:
    void onStartButtonClicked();
    void onSettingsButtonClicked();
    void onMinimizeButtonClicked();
    void onCloseButtonClicked();
    void updateProgress();
     // 显示图片的槽函数
    void showImage(const QString &imagePath) {
        // 加载图片
        QPixmap pixmap(imagePath);
        if (pixmap.isNull()) {
            imageLabel->setText("Failed to load image!"); // 如果图片加载失败，显示错误信息
        } else {
	    centralWidget->resize(this->width(),this->height()-30);
	    centralWidget->move(0,30);
            imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio)); // 缩放图片并显示
	    imageLabel->show();

    //centralWidget->setLayout(nullptr);
    // 设置 QWidget 的布局
    //centralWidget->setLayout(layout);

    // 显示 QWidget
    //centralWidget->move(0,0);
    //centralWidget->show();
        }
    }
  
signals:
   // 自定义信号
    void imageSignal(const QString &imagePath);

private:
   
    void initProgress();
    void createTitleBar(); // 创建自定义标题栏

    QLabel *imageLabel;
    QPushButton *startButton;
    QPushButton *settingsButton;
    QPushButton *minimizeButton; // 最小化按钮
    QPushButton *closeButton;    // 关闭按钮
    QPoint dragPosition; // 用于窗口拖动
    QWidget *centralWidget;
    QVBoxLayout * centralLayout;

    QProgressBar *progressBar; // 进度条
    bool downloading,extracting,installing; // 不同进度标识
    int recyleNum = 0; // 重复的次数
    QLabel *statusLabel;       // 状态标签
    QTimer *timer;             // 定时器
    int currentProgress;       // 当前进度
};

#endif // MAINWINDOW_H
