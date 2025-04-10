#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QEvent>
#include <QProgressDialog>
#include "shapeButton.h"
#include <QMouseEvent>
#include "download.h"
#include "install_worker.h"
#include "start_worker.h"
#include <QMutex>
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
#include "shellUtils.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
	QPoint dragPosition;    // Add this to store the drag position

protected:
    void changeEvent(QEvent *event) override {
	if (event->type() == QEvent::ActivationChange) {
      	    if (!isActiveWindow()) {
		clearFocus();
	    }
	}
	    QMainWindow::changeEvent(event);
    }
    void mousePressEvent(QMouseEvent *event) override {
	if (event->button() == Qt::LeftButton) {
	    dragPosition = event->globalPos() - frameGeometry().topLeft();
            event->accept();
        }
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (event->buttons() & Qt::LeftButton) {
            move(event->globalPos() - dragPosition);
            event->accept();
        }
    }
    bool eventFilter(QObject* obj, QEvent* event) override {
        if (event->type() == QEvent::WindowActivate) {
        }
        return QObject::eventFilter(obj, event);
    }
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onInstallWorkerFinishedError(QString err);
    void onSettingsButtonClicked();
    void onMinimizeButtonClicked();
    void onCloseButtonClicked();
    void updateProgress();
    void onMessageReceived(const QString &message, bool withAction); // 定义槽
     // 显示图片的槽函数
    void onRunEnded();
public slots:
    void showImage(bool immediately=false);
    void cancelInstalling();
	
signals:
   // 自定义信号
    void imageSignal(bool immediately=false);
    void sendStatusUpdateMessage(const QString status);

protected:
void mouseDoubleClickEvent(QMouseEvent *event) override {
    Logger::log(Logger::INFO,"double click mainwindow");
    if ( !shellUtils::isOpenfdeClosed){
	    shellUtils::switchToOpenfdeDesktop();
    }
}
private:
   
    void ceaseInstalling();
    int initProgress();
    void createTitleBar(); // 创建自定义标题栏

    QLabel *imageLabel;
    QProgressDialog *progress;
    Worker *installWorker;
    StartWorker *startWorker;
    QThread *startThread;
    QThread *workThread;
    QPushButton *settingsButton;
    QPushButton *minimizeButton; // 最小化按钮
    QPushButton *closeButton;    // 关闭按钮
    QWidget *centralWidget;
    QVBoxLayout * centralLayout;
    ShapeButton *btn;

    FileDownloader *downloader;
    QProgressBar *progressBar; // 进度条
    QLabel *statusLabel;       // 状态标签
    QTimer *timer;             // 定时器
    int currentProgress;       // 当前进度
    bool extracting, installing, downloading, preparing;
    QProcess *cmdProcess;
    //增加一个互斥锁
    QMutex ImageMutex;
    //增加一个记录时间戳的变量
    qint64 lastShowTime;
    QPushButton *cancelButton;
};



// 自定义事件过滤器类
class WindowActivateFilter : public QObject {
public:
    WindowActivateFilter(QObject* parent = nullptr) : QObject(parent) {}

protected:
    bool eventFilter(QObject* obj, QEvent* event) override {
        if (event->type() == QEvent::WindowActivate) {
	    MainWindow* myWidget = qobject_cast<MainWindow*>(obj);
            if (myWidget) {
                // 调用 MyWidget 的自定义方法
                myWidget->showImage();
            }
            // 在这里可以添加类似恢复操作的代码
        }
        return QObject::eventFilter(obj, event);
    }
};

#endif // MAINWINDOW_H
