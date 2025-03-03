#include <QApplication>
#include <QWidget>
#include <QTimer>
#include <QPushButton>
#include <QPainter>
#include <QBitmap>
#include <QVBoxLayout>
#include <QMainWindow>
#include "start_worker.h"
#include <QMutex>

static const QString button_stop_status = "stop";
static const QString button_start_status = "start";

// 自定义圆形背景和形状按钮的类
class ShapeButton : public QWidget {
	Q_OBJECT
public:
    explicit ShapeButton(QWidget *parent = nullptr) ;
    void toggleToStatus(QString status,bool send = false);
signals:
    void sendMessage(const QString &message, bool withAction); // 定义信号

public slots:
    void receiveStatusUpdateMessage(const QString status);

protected:
    void paintEvent(QPaintEvent *event) override {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        // 绘制圆形背景
        // painter.setBrush(QColor(200, 255, 200)); // 更清淡的绿色
        //painter.setBrush(QColor(144, 238, 144)); // 淡绿色
        //painter.setBrush(QColor(173, 216, 230)); // 淡蓝色
        painter.setBrush(QColor(173,225,225,128)); // 淡蓝色,半透明
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(50, 50, 100, 100); // 绘制圆形
	}
private slots:
    void toggleButtonShape();
   

private:
    QPushButton *shapeButton; // 形状切换按钮
    bool isTriangle; // 当前形状是否为三角形
    void updateButtonShape(bool send = true);
    QMutex mutex;
    QTimer *timer;
};

