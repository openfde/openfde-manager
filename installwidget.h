#ifndef INSTALLWIDGET_H
#define INSTALLWIDGET_H

#include <QWidget>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QTimer>

class InstallWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InstallWidget(QWidget *parent = nullptr);
    ~InstallWidget();

private slots:
    void updateProgress(); // 更新进度条

private:
    QProgressBar *progressBar; // 进度条
    bool download,extra,install; // 不同进度标识
    int recyleNum =0; // 不同进度标识
    QLabel *statusLabel;       // 状态标签
    QTimer *timer;             // 定时器
    int currentProgress;       // 当前进度
};

#endif // INSTALLWIDGET_H
