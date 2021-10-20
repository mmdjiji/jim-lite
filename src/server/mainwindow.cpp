#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() &~ Qt::WindowMaximizeButtonHint);

    // Protocol
    protocol = new Protocol();
    connect(protocol, SIGNAL(sig_logger(QString)), this, SLOT(logger(QString))); // Feed back of logger from class Protocol.

    protocol->listen(3000);

}

MainWindow::~MainWindow()
{
    delete protocol;
    delete ui;
}

void MainWindow::logger(QString value) {
    ui->logger->setText(ui->logger->toPlainText() + value + "\n");
    QTextCursor cursor = ui->logger->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->logger->setTextCursor(cursor);
}
