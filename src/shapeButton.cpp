#include "shapeButton.h"

// 自定义圆形背景和形状按钮的类
CircleWidgetWithButton::CircleWidgetWithButton(QWidget *parent):  QWidget(parent), isTriangle(true) {
    setFixedSize(200, 200); // 设置窗口大小
    setWindowOpacity(0.1); // 设置按钮整体透明度为 80%
    // 创建形状切换按钮
    shapeButton = new QPushButton(this);
    shapeButton->setFixedSize(60, 60); // 设置按钮大小
    shapeButton->move(70, 70); // 将按钮放置在圆形背景的中心
    updateButtonShape(); // 初始形状为三角形
			     timer = new QTimer(this);
    timer->setSingleShot(true);
    shapeButton->setProperty("isClickable", true);

    // 连接按钮的点击信号到槽函数
    connect(shapeButton, &QPushButton::clicked, this, [this]() {
        if (shapeButton->property("isClickable").toBool()) {
            shapeButton->setProperty("isClickable", false);
            timer->start(500); // 500ms cooldown
            toggleButtonShape();
        }
    });
    connect(timer, &QTimer::timeout, this, [this]() {
        shapeButton->setProperty("isClickable", true);
    });
    shapeButton->setMouseTracking(true);
    shapeButton->setStyleSheet(
		    "QPushButton {"
		    " background-color: skyblue;"
		    "}"
		    );
}

void CircleWidgetWithButton::toggleToStatus(QString status, bool send){
    if (status == button_stop_status){
        isTriangle = true;
    }else {
        isTriangle = false;
    }
    updateButtonShape(send);
}

void CircleWidgetWithButton::toggleButtonShape() {
    isTriangle = !isTriangle; // 切换形状
    updateButtonShape(true); // 更新按钮形状
}

const QString Button_stop = "stop";
const QString Button_start = "start";

void CircleWidgetWithButton::updateButtonShape(bool withAction) {
    // 更新按钮的遮罩
    QBitmap mask(shapeButton->size());
    mask.clear();
    QPainter painter(&mask);
    painter.setBrush(Qt::color1);
    mutex.lock();
    if (isTriangle) {
        // 三角形遮罩
        QPolygonF triangle;
        triangle << QPointF(15, 50) << QPointF(15, 10) << QPointF(60,30);
        painter.drawPolygon(triangle);
	if (!withAction) {
			
	}else{
        	emit sendMessage(button_stop_status,withAction);
	}
    } else {
        // 矩形遮罩
        painter.drawRect(10, 10, 40, 40);
	if (!withAction) {

	}else {
		emit sendMessage(button_start_status,withAction);
	}
       
    }
    mutex.unlock();
    shapeButton->setMask(mask);
    shapeButton->update(); // 触发按钮重绘
}

void CircleWidgetWithButton::receiveStatusUpdateMessage(const QString status){
    toggleToStatus(status,false);
}



