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
    this->setFixedSize(953, 408);
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    InstrumentCluster *cluster = new InstrumentCluster(this);

    QVBoxLayout *layout = new QVBoxLayout(ui->clusterWidget);
    layout->addWidget(cluster);
}

MainWindow::~MainWindow()
{
    delete ui;
}
