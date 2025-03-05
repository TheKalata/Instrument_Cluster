#include "instrumentcluster.h"
#include <QPainter>
#include <QTimer>
#include <cmath>
#include <QtMath>
#include <QDebug>
#include <QUdpSocket>
#include <QStringList>

InstrumentCluster::InstrumentCluster(QWidget *parent)
    : QWidget(parent), 
      speed(0),
      rpm(0),
      leftIndicator(false),
      leftIndicatorVisible(false),
      rightIndicator(false),
      rightIndicatorVisible(false),
      highBeams(false),
      dimpBeam(false)
{
    setFocusPolicy(Qt::StrongFocus);
    this->setFocus();

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::LocalHost, 5006);
    connect(udpSocket, &QUdpSocket::readyRead, this, &InstrumentCluster::processIncomingData);

    QTimer *indicatorTimer = new QTimer(this);
    connect(indicatorTimer, &QTimer::timeout, this, [this]() {
        if (leftIndicator) {
            leftIndicatorVisible = !leftIndicatorVisible;
            update();
        }
        if (rightIndicator) {
            rightIndicatorVisible = !rightIndicatorVisible;
            update();
        }
    });
    indicatorTimer->start(500);
}

void InstrumentCluster::processIncomingData(){
    while (udpSocket->hasPendingDatagrams()){
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        QList<QByteArray> dataList = datagram.split(',');

        if (dataList.size() >= 6){
            speed = dataList[0].toFloat();
            rpm = dataList[1].toInt();
            leftIndicator = dataList[2].toInt();
            rightIndicator = dataList[3].toInt();
            dimpBeam = dataList[4].toInt();
            highBeams = dataList[5].toInt();
        }

        update();
    }
}

void InstrumentCluster::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int speedCenterX = 227;
    int rpmCenterX = width() - speedCenterX;
    int centerY = height() / 2;
    int radius = 160;
    int needleLength = radius - 30;
    int needleHeight = 110;

    // ---- ФУНКЦИЯ ЗА РИСУВАНЕ НА ТРИЪГЪЛНА СТРЕЛКА ----
    auto drawNeedle = [&](QPainter &painter, int centerX, double angle, QColor color) {
        double rad = qDegreesToRadians(angle);

        QPointF tip(centerX + needleLength * std::cos(rad),
                    centerY + needleLength * std::sin(rad));

        QPointF baseLeft(centerX + (needleLength - needleHeight) * std::cos(rad + M_PI / 12),
                         centerY + (needleLength - needleHeight) * std::sin(rad + M_PI / 12));

        QPointF baseRight(centerX + (needleLength - needleHeight) * std::cos(rad - M_PI / 12),
                          centerY + (needleLength - needleHeight) * std::sin(rad - M_PI / 12));

        QPolygonF needle;
        needle << tip << baseLeft << baseRight;

        painter.setBrush(color);
        painter.setPen(Qt::NoPen);
        painter.drawPolygon(needle);
    };

    // --СКОРОСТОМЕР--
    painter.setPen(QPen(Qt::white, 2));

    for (int i = 10; i <= 230; i += 20){
        double angle = (i / 240.0) * 270.0 + 135.0; 
        double rad = qDegreesToRadians(angle);

        int outerX = speedCenterX + radius * std::cos(rad);
        int outerY = centerY + radius * std::sin(rad);

        int innerX = speedCenterX + (radius - 10) * std::cos(rad);
        int innerY = centerY + (radius - 10) * std::sin(rad);

        if (i == 50){
            painter.setPen(QPen(Qt::red, 2));
            painter.drawLine(innerX, innerY, outerX, outerY);
            painter.setPen(QPen(Qt::white, 2));
        }
        else{
            painter.drawLine(innerX, innerY, outerX, outerY);
        }
    }

    painter.setPen(QPen(Qt::white, 4));
    painter.drawEllipse(speedCenterX - radius, centerY - radius, 2 * radius, 2 * radius);

    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(speedCenterX - 20, (this->height() / 2) - 20, 40, 40);

    for (int i = 0; i <= 240; i += 20) {
        double angle = (i / 240.0) * 270.0 + 135.0; 
        double rad = qDegreesToRadians(angle);

        int textRadius = radius - 25;

        int textX = speedCenterX + textRadius * std::cos(rad) - 15;
        int textY = centerY + textRadius * std::sin(rad) + 5;

        QFont font = painter.font();
        font.setPointSize(14);
        font.setBold(true);
        painter.setFont(font);

        painter.setPen(Qt::white);
        painter.drawText(textX, textY, QString::number(i));
    }

    QFont font = painter.font();
    font.setBold(false);
    painter.setFont(font);

    double speedAngle = (speed / 240.0) * 270.0 + 135.0;
    drawNeedle(painter, speedCenterX, speedAngle, Qt::white);
    painter.setBrush(Qt::NoBrush); 

    // --ОБОРОТОМЕТЪР--
    painter.setPen(QPen(Qt::white, 3));

    for (int i = 500; i <= 8000; i += 1000){
        double angle = (i / 8000.0) * 270.0 + 135.0;  
        double rad = qDegreesToRadians(angle);

        int outerX = rpmCenterX + radius * std::cos(rad);
        int outerY = centerY + radius * std::sin(rad);

        int innerX = rpmCenterX + (radius - 15) * std::cos(rad);
        int innerY = centerY + (radius - 15) * std::sin(rad);

        if (i > 6000){
            painter.setPen(QPen(Qt::red, 3));
            painter.drawLine(innerX, innerY, outerX, outerY);
            painter.setPen(QPen(Qt::white, 3));
        }
        else{
            painter.drawLine(innerX, innerY, outerX, outerY);
        }
    }

    painter.setPen(QPen(Qt::white, 4));
    painter.drawEllipse(rpmCenterX - radius, centerY - radius, 2 * radius, 2 * radius);

    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rpmCenterX - 20, (this->height() / 2) - 20, 40, 40);

    for (int i = 0; i <= 8000; i += 1000) {
        double angle = (i / 8000.0) * 270.0 + 135.0; 
        double rad = qDegreesToRadians(angle);

        int textRadius = radius - 20; 

        int textX = rpmCenterX + textRadius * std::cos(rad) - 5;
        int textY = centerY + textRadius * std::sin(rad) + 10;

        QFont font = painter.font();
        font.setBold(true);
        font.setPointSize(18);
        painter.setFont(font);

        if (i < 6000) {
            painter.setPen(Qt::white);
            painter.drawText(textX, textY, QString::number(i / 1000));
        }
        else {
            painter.setPen(Qt::red);
            painter.drawText(textX, textY, QString::number(i / 1000));
        }
    }

    double rpmAngle = (rpm / 8000.0) * 270.0 + 135.0;
    drawNeedle(painter, rpmCenterX, rpmAngle, Qt::white);

    // --ЛЯВ МИГАЧ--
    if (leftIndicator && leftIndicatorVisible) {
        painter.setBrush(Qt::green);
        painter.setPen(Qt::NoPen);

        QPolygonF leftIndicator;
        leftIndicator << QPointF(width() / 2 - 150, 30)
                      << QPointF(width() / 2 - 125, 15)
                      << QPointF(width() / 2 - 125, 45);

        QPolygonF leftSquare;
        leftSquare << QPointF(width() / 2 - 125, 22)
                   << QPointF(width() / 2 - 125, 38)
                   << QPointF(width() / 2 - 109, 38)
                   << QPointF(width() / 2 - 109, 22);

        painter.drawPolygon(leftIndicator);
        painter.drawPolygon(leftSquare);
    }

    // --ДЕСЕН МИГАЧ--
    if (rightIndicator && rightIndicatorVisible) {
        painter.setBrush(Qt::green); 
        painter.setPen(Qt::NoPen);  

        QPolygonF rightIndicator;
        rightIndicator << QPointF(width() / 2 + 150, 30)
                      << QPointF(width() / 2 + 125, 15)
                      << QPointF(width() / 2 + 125, 45);

        QPolygonF rightSquare;
        rightSquare << QPointF(width() / 2 + 125, 22)
                   << QPointF(width() / 2 + 125, 38)
                   << QPointF(width() / 2 + 109, 38)
                   << QPointF(width() / 2 + 109, 22);

        painter.drawPolygon(rightIndicator);
        painter.drawPolygon(rightSquare);
    }

    // --ФАРОВЕ--
    if (highBeams){
        int x = width() / 2 + 50;
        int y = height() / 2 + 60;
        int width = 20;
        int height = 20;

        QPen pen(Qt::blue, 2);
        painter.setPen(pen);

        QRectF dRectL(x + width * 0.5, y, width * 0.2, height);
        painter.drawArc(dRectL, 90 * 16, 180 * 16);

        QRectF dRectR(x + width * 0.2, y, width, height);
        painter.drawArc(dRectR, 270 * 16, 180 * 16);

        int lineSpacing = height / 5; 
        int lineLength = width * 0.4;
        int offsetY = height * 0.05;

        for (int i = 0; i < 4; ++i) {
            int lineY = y + i * (height / 4.0) + lineSpacing - offsetY; 
            QPointF start(x, lineY);
            QPointF end(x + lineLength, lineY);
            painter.drawLine(start, end);
        }
    }

    if (dimpBeam){
        int x = width() / 2 + 50;
        int y = height() / 2 + 60;
        int width = 20;
        int height = 20;

        QPen pen(Qt::green, 2);
        painter.setPen(pen);

        QRectF dRectL(x + width * 0.5, y, width * 0.2, height);
        painter.drawArc(dRectL, 90 * 16, 180 * 16);

        QRectF dRectR(x + width * 0.2, y, width, height);
        painter.drawArc(dRectR, 270 * 16, 180 * 16);

        int lineSpacing = height / 5;
        int lineLength = width * 0.4;
        int offsetY = height * 0.05;

        for (int i = 0; i < 4; ++i) {
            int lineY = y + i * (height / 4.0) + lineSpacing - offsetY;
            QPointF start(x, lineY + lineSpacing);
            QPointF end(x + lineLength, lineY);
            painter.drawLine(start, end);
        }
    }
}
