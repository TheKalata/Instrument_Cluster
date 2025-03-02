#ifndef INSTRUMENTCLUSTER_H
#define INSTRUMENTCLUSTER_H

#include <QWidget>
#include <QUdpSocket>

class InstrumentCluster : public QWidget {
    Q_OBJECT

public:
    explicit InstrumentCluster(QWidget *parent = nullptr);
    ~InstrumentCluster() override = default;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void processIncomingData();

private:
    int speed;
    int rpm;
    bool leftIndicator;
    bool leftIndicatorVisible;
    bool rightIndicator;
    bool rightIndicatorVisible;
    bool highBeams;
    bool dimpBeam;

    QUdpSocket *udpSocket;
};

#endif
