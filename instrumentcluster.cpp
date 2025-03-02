#include "instrumentcluster.h"
#include <QPainter>
#include <QTimer>
#include <cmath>
#include <QtMath>
#include <QDebug>
#include <QUdpSocket>
#include <QStringList>

InstrumentCluster::InstrumentCluster(QWidget *parent)
    : QWidget(parent), speed(0), rpm(0), leftIndicator(false), leftIndicatorVisible(false), rightIndicator(false), rightIndicatorVisible(false), highBeams(false), dimpBeam(false) // Инициализиране на проемнливите
{
    setFocusPolicy(Qt::StrongFocus); //Позволява прихващането на клавиши
    this->setFocus();

    udpSocket = new QUdpSocket(this);
    udpSocket->bind(QHostAddress::LocalHost, 5006);
    connect(udpSocket, &QUdpSocket::readyRead, this, &InstrumentCluster::processIncomingData);

    // Таймер за мигачите
    QTimer *indicatorTimer = new QTimer(this);
    connect(indicatorTimer, &QTimer::timeout, this, [this]() {
        if (leftIndicator) {
            leftIndicatorVisible = !leftIndicatorVisible; // Превключване на видимостта
            update(); // Обновява само когато мигачът е включен
        }
        if (rightIndicator) {
            rightIndicatorVisible = !rightIndicatorVisible;
            update();
        }
    });
    indicatorTimer->start(500); // Мига на всеки 500 ms
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

// Метод за рисуване
void InstrumentCluster::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Параметри
    int speedCenterX = 227;
    int rpmCenterX = width() - speedCenterX;
    int centerY = height() / 2;
    int radius = 160;
    int needleLength = radius - 30; //дължината на височината на стрелката
    int needleHeight = 110; //дължина на основата на стрелката

    // ---- ФУНКЦИЯ ЗА РИСУВАНЕ НА ТРИЪГЪЛНА СТРЕЛКА ----
    auto drawNeedle = [&](QPainter &painter, int centerX, double angle, QColor color) {
        double rad = qDegreesToRadians(angle);

        QPointF tip(centerX + needleLength * std::cos(rad),
                    centerY + needleLength * std::sin(rad)); // Връх на стрелката

        QPointF baseLeft(centerX + (needleLength - needleHeight) * std::cos(rad + M_PI / 12),
                         centerY + (needleLength - needleHeight) * std::sin(rad + M_PI / 12)); // Ляв край на основата

        QPointF baseRight(centerX + (needleLength - needleHeight) * std::cos(rad - M_PI / 12),
                          centerY + (needleLength - needleHeight) * std::sin(rad - M_PI / 12)); // Десен край на основата

        QPolygonF needle;
        needle << tip << baseLeft << baseRight;

        painter.setBrush(color); // Запълване с цвят
        painter.setPen(Qt::NoPen); // Без контур
        painter.drawPolygon(needle);
    };

    // --СКОРОСТОМЕР--

    // Линии на скоростометъра
    painter.setPen(QPen(Qt::white, 2));

    for (int i = 10; i <= 230; i += 20){
        double angle = (i / 240.0) * 270.0 + 135.0;  // Изчисляваме ъгъла за дадената скорост
        double rad = qDegreesToRadians(angle);

        // Координати на края на линията (външен ръб на циферблата)
        int outerX = speedCenterX + radius * std::cos(rad);
        int outerY = centerY + radius * std::sin(rad);

        // Координати на началото на линията (вътрешен ръб на циферблата)
        int innerX = speedCenterX + (radius - 10) * std::cos(rad); // 10 пиксела по-къса
        int innerY = centerY + (radius - 10) * std::sin(rad);

        // Рисуване на линията
        if (i == 50){
            painter.setPen(QPen(Qt::red, 2));
            painter.drawLine(innerX, innerY, outerX, outerY);
            painter.setPen(QPen(Qt::white, 2));
        }
        else{
            painter.drawLine(innerX, innerY, outerX, outerY);
        }
    }

    // Рисуване на циферблат
    painter.setPen(QPen(Qt::white, 4));
    painter.drawEllipse(speedCenterX - radius, centerY - radius, 2 * radius, 2 * radius);

    //Рисуване на центъра на циферблата
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(speedCenterX - 20, (this->height() / 2) - 20, 40, 40);

    // Числа на скоростометъра
    for (int i = 0; i <= 240; i += 20) {  // Интервали от 20 км/ч
        double angle = (i / 240.0) * 270.0 + 135.0;  // Изчисляваме ъгъла за дадената скорост
        double rad = qDegreesToRadians(angle);

        int textRadius = radius - 25;  // Разстояние на числата от центъра

        int textX = speedCenterX + textRadius * std::cos(rad) - 15;  // -10 за центриране
        int textY = centerY + textRadius * std::sin(rad) + 5;   // +5 за центриране

        //Настройка на шифт
        QFont font = painter.font();
        font.setPointSize(14); //Размер на числата
        font.setBold(true);
        painter.setFont(font);

        painter.setPen(Qt::white);
        painter.drawText(textX, textY, QString::number(i));  // Рисуваме числото
    }

    QFont font = painter.font();
    font.setBold(false);
    painter.setFont(font);

    // Стрелка на скоростометър
    double speedAngle = (speed / 240.0) * 270.0 + 135.0;
    drawNeedle(painter, speedCenterX, speedAngle, Qt::white);
    painter.setBrush(Qt::NoBrush); // Връщаме четката в нормално състояние

    // --ОБОРОТОМЕТЪР--

    // Линии на оборотометъра
    painter.setPen(QPen(Qt::white, 3));

    for (int i = 500; i <= 8000; i += 1000){
        double angle = (i / 8000.0) * 270.0 + 135.0;  // Изчисляваме ъгъла за дадените обороти
        double rad = qDegreesToRadians(angle);

        // Координати на края на линията (външен ръб на циферблата)
        int outerX = rpmCenterX + radius * std::cos(rad);
        int outerY = centerY + radius * std::sin(rad);

        // Координати на началото на линията (вътрешен ръб на циферблата)
        int innerX = rpmCenterX + (radius - 15) * std::cos(rad);
        int innerY = centerY + (radius - 15) * std::sin(rad);

        // Рисуване на линията
        if (i > 6000){
            painter.setPen(QPen(Qt::red, 3));
            painter.drawLine(innerX, innerY, outerX, outerY);
            painter.setPen(QPen(Qt::white, 3));
        }
        else{
            painter.drawLine(innerX, innerY, outerX, outerY);
        }
    }

    // Рисуване на циферблат
    painter.setPen(QPen(Qt::white, 4));
    painter.drawEllipse(rpmCenterX - radius, centerY - radius, 2 * radius, 2 * radius);

    // Рисуване на центъра на оборотометъра
    painter.setBrush(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rpmCenterX - 20, (this->height() / 2) - 20, 40, 40);

    // Числа на оборотометъра
    for (int i = 0; i <= 8000; i += 1000) {  // Интервали от 1000 RPM
        double angle = (i / 8000.0) * 270.0 + 135.0;  // Изчисляваме ъгъла за дадените обороти
        double rad = qDegreesToRadians(angle);

        int textRadius = radius - 20;  // Разстояние на числата от центъра

        int textX = rpmCenterX + textRadius * std::cos(rad) - 5;
        int textY = centerY + textRadius * std::sin(rad) + 10;

        QFont font = painter.font();
        font.setBold(true);
        font.setPointSize(18); //Размер на числата
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

    // Стрелка на оборотометър
    double rpmAngle = (rpm / 8000.0) * 270.0 + 135.0;
    drawNeedle(painter, rpmCenterX, rpmAngle, Qt::white);

    // --ЛЯВ МИГАЧ--
    if (leftIndicator && leftIndicatorVisible) {
        painter.setBrush(Qt::green);
        painter.setPen(Qt::NoPen);

        // Рисуване на мигач (малък триъгълник от лявата страна и квадрат до него)
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
        painter.setBrush(Qt::green); // Жълт цвят за мигача
        painter.setPen(Qt::NoPen);    // Без контур

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

    // Дълги светлини
    if (highBeams){
        int x = width() / 2 + 50;
        int y = height() / 2 + 60;
        int width = 20;
        int height = 20;

        QPen pen(Qt::blue, 2);
        painter.setPen(pen);

        // Рисуване на дъгите
        QRectF dRectL(x + width * 0.5, y, width * 0.2, height);
        painter.drawArc(dRectL, 90 * 16, 180 * 16);

        QRectF dRectR(x + width * 0.2, y, width, height);
        painter.drawArc(dRectR, 270 * 16, 180 * 16);

        // Рисуване на светлинните лъчи (линии отляво)
        int lineSpacing = height / 5; // Разстояние между линиите
        int lineLength = width * 0.4; // Дължина на всяка линия
        int offsetY = height * 0.05;

        for (int i = 0; i < 4; ++i) {
            int lineY = y + i * (height / 4.0) + lineSpacing - offsetY; // Вертикална позиция на линията
            QPointF start(x, lineY);
            QPointF end(x + lineLength, lineY);
            painter.drawLine(start, end);
        }
    }

    // Къси светлини
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
