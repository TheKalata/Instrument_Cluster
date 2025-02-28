#ifndef INSTRUMENTCLUSTER_H
#define INSTRUMENTCLUSTER_H

#include <QWidget>

class InstrumentCluster : public QWidget {
    Q_OBJECT

public:
    explicit InstrumentCluster(QWidget *parent = nullptr); // Конструктор
    ~InstrumentCluster() override = default;

protected:
    void paintEvent(QPaintEvent *event) override; // Метод за рисуване
    void keyPressEvent(QKeyEvent *event) override; //Метод за управление на клавиши

private:
    int speed;
    int rpm;
    bool leftIndicator;
    bool leftIndicatorVisible;
    bool rightIndicator;
    bool rightIndicatorVisible;
    bool highBeams;
    bool dimpBeam;
};

#endif // INSTRUMENTCLUSTER_H
