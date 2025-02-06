#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QPainter>
#include <QBitmap>
#include <QVBoxLayout>
#include <QMainWindow>

// 自定义圆形背景和形状按钮的类
class CircleWidgetWithButton : public QWidget {
	Q_OBJECT
public:
    explicit CircleWidgetWithButton(QWidget *parent = nullptr) ;
signals:
    void sendMessage(const QString &message); // 定义信号

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
    void updateButtonShape();
};

