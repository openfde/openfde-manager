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

    // 连接按钮的点击信号到槽函数
    connect(shapeButton, &QPushButton::clicked, this, &CircleWidgetWithButton::toggleButtonShape);
}

void CircleWidgetWithButton::toggleToStatus(QString status){
    if (status == button_stop_status){
        isTriangle = true;
    }else {
        isTriangle = false;
    }
    updateButtonShape(false);
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
    if (isTriangle) {
        // 三角形遮罩
        QPolygonF triangle;
        //triangle << QPointF(50, 10) << QPointF(10, 30) << QPointF(50, 50);
        triangle << QPointF(15, 50) << QPointF(15, 10) << QPointF(60,30);
        painter.drawPolygon(triangle);
        sendMessage(button_stop_status,withAction);
    } else {
        // 矩形遮罩
        painter.drawRect(10, 10, 40, 40);
        sendMessage(button_start_status,withAction);
       
    }
    shapeButton->setMask(mask);
//shapeButton->setStyleSheet("background-color: skyblue; border: none;");
    shapeButton->setStyleSheet("background-color: skyblue;");
    shapeButton->update(); // 触发按钮重绘
}


