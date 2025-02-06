#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "shapeButton.h"
#include "download.h"
#include "installworker.h"
#include <QMainWindow>
#include <QPushButton>
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <QProcess>
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

private slots:
    void onSettingsButtonClicked();
    void onMinimizeButtonClicked();
    void onCloseButtonClicked();
    void updateProgress();
    void onMessageReceived(const QString &message); // 定义槽
     // 显示图片的槽函数
    void showImage(const QString &imagePath);
    void onScriptFinished(const QByteArray &output, const QByteArray &error);
  
signals:
   // 自定义信号
    void imageSignal(const QString &imagePath);

private:
   
    void initProgress();
    void createTitleBar(); // 创建自定义标题栏

    QLabel *imageLabel;
    Worker *installWorker;
    QThread *workThread;
    QPushButton *settingsButton;
    QPushButton *minimizeButton; // 最小化按钮
    QPushButton *closeButton;    // 关闭按钮
    QWidget *centralWidget;
    QVBoxLayout * centralLayout;
    CircleWidgetWithButton *btn;

    FileDownloader *downloader;
    QProgressBar *progressBar; // 进度条
    QLabel *statusLabel;       // 状态标签
    QTimer *timer;             // 定时器
    int currentProgress;       // 当前进度
    bool extracting , installing, downloading;
   QProcess *cmdProcess;
};

#endif // MAINWINDOW_H
