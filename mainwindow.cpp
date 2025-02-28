#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "instrumentcluster.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setStyleSheet("background-color: black;");
    this->setFixedSize(953, 408); //ratio = 21:9
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Създаване на инструменталния клъстер
    InstrumentCluster *cluster = new InstrumentCluster(this);

    // Добавяне към QWidget от дизайнера (например clusterWidget)
    QVBoxLayout *layout = new QVBoxLayout(ui->clusterWidget);
    layout->addWidget(cluster);
}

MainWindow::~MainWindow()
{
    delete ui;
}
