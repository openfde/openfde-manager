#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QEvent>
#include "shapeButton.h"
#include "download.h"
#include "install_worker.h"
#include "start_worker.h"
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

protected:
    bool eventFilter(QObject* obj, QEvent* event) override {
    qDebug() << "Window is activated before";
        if (event->type() == QEvent::WindowActivate) {
            qDebug() << "Window is activated";
            // 在这里可以添加类似恢复操作的代码
        }
        return QObject::eventFilter(obj, event);
    }
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onSettingsButtonClicked();
    void onMinimizeButtonClicked();
    void onCloseButtonClicked();
    void updateProgress();
    void onMessageReceived(const QString &message, bool withAction); // 定义槽
     // 显示图片的槽函数
    void onRunEnded();
public slots:
    void showImage();
signals:
   // 自定义信号
    void imageSignal();

private:
   
    void initProgress();
    void createTitleBar(); // 创建自定义标题栏

    QLabel *imageLabel;
    Worker *installWorker;
    StartWorker *startWorker;
    QThread *startThread;
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



// 自定义事件过滤器类
class WindowActivateFilter : public QObject {
public:
    WindowActivateFilter(QObject* parent = nullptr) : QObject(parent) {}

protected:
    bool eventFilter(QObject* obj, QEvent* event) override {
        if (event->type() == QEvent::WindowActivate) {
            qDebug() << "Window is activated";
	    MainWindow* myWidget = qobject_cast<MainWindow*>(obj);
            if (myWidget) {
                // 调用 MyWidget 的自定义方法
                myWidget->showImage("");
            }
            // 在这里可以添加类似恢复操作的代码
        }
        return QObject::eventFilter(obj, event);
    }
};

#endif // MAINWINDOW_H
