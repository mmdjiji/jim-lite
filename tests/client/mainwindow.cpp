#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    socket = new QWebSocket();
}

MainWindow::~MainWindow()
{
    delete socket;
    delete ui;
}

void MainWindow::on_connect_clicked()
{
    socket->open(QUrl(ui->wsAddr->text()));

    connect(socket, SIGNAL(connected()), this, SLOT(onConnected()));

}

void MainWindow::onConnected() {
    ui->textEdit->append("连接成功\n");
    connect(socket, SIGNAL(textMessageReceived(QString)), this, SLOT(onTextMessageReceived(QString)));
}

void MainWindow::onTextMessageReceived(QString data) {
    ui->textEdit->append(data + "\n");
}

void MainWindow::on_send_clicked()
{
    socket->sendTextMessage(ui->lineEdit->text());
}
