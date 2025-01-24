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

private slots:
    void onStartButtonClicked();
    void onSettingsButtonClicked();
    void onMinimizeButtonClicked();
    void onCloseButtonClicked();
    void updateProgress();
     // 显示图片的槽函数
    void showImage(const QString &imagePath) {
        // 加载图片
qDebug()<< "show image";
        QPixmap pixmap(imagePath);
        if (pixmap.isNull()) {
            imageLabel->setText("Failed to load image!"); // 如果图片加载失败，显示错误信息
        } else {
            imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio)); // 缩放图片并显示
		centralWidget->resize(800,600);
		 // 创建一个垂直布局
		QWidget * image = new QWidget(this);
		image->setFixedSize(800,600);
    QVBoxLayout *layout = new QVBoxLayout(image);

    // 将 QLabel 添加到布局中
    layout->addWidget(imageLabel);

    image->show();

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

    QProgressBar *progressBar; // 进度条
    bool downloading,extracting,installing; // 不同进度标识
    int recyleNum = 0; // 重复的次数
    QLabel *statusLabel;       // 状态标签
    QTimer *timer;             // 定时器
    int currentProgress;       // 当前进度
};

#endif // MAINWINDOW_H
