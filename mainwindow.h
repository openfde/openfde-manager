#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include "installwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void moveEvent(QMoveEvent *event) override; // 重写 moveEvent 函数

private slots:
    void onStartButtonClicked();
    void onSettingsButtonClicked();
    void onMinimizeButtonClicked();
    void onCloseButtonClicked();

private:
    void createTitleBar(); // 创建自定义标题栏

    QLabel *imageLabel;
    QPushButton *startButton;
    QPushButton *settingsButton;
    QPushButton *minimizeButton; // 最小化按钮
    QPushButton *closeButton;    // 关闭按钮
    InstallWidget *installWidget; // 安装进度窗口
    QPoint dragPosition; // 用于窗口拖动
};

#endif // MAINWINDOW_H
