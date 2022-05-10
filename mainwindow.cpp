#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QKeyEvent>
#include <QNetworkDatagram>

#include "../server/protocol.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    udp_socket(this)
{
    ui->setupUi(this);

    udp_socket.bind(QHostAddress::LocalHost, 7777);
    connect(&udp_socket, &QUdpSocket::readyRead,
            this, &MainWindow::recieve);

    graph = ui->graph;
    layout = ui->layout;
    combo_filter = ui->combo_filter;

    for (const auto &i : combo_elements.keys()) {
        combo_filter->addItem(i);
    }

    ui->centralwidget->setLayout(layout);
}

MainWindow::~MainWindow()
{
    udp_socket.close();

    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    qInfo() << ui->centralwidget->geometry();
    qInfo() << ui->layout->geometry();
}

void MainWindow::recieve()
{
    dgram dg;
    while (udp_socket.hasPendingDatagrams()) {
        auto readed = udp_socket.readDatagram((char *)&dg, sizeof(dgram));
        // process it
        if (readed == sizeof(dgram))
            graph->add_point(dg, combo_elements[combo_filter->currentText()]);
    }
}

