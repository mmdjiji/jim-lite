#include "loadwindow.h"
#include "ui_loadwindow.h"

LoadWindow::LoadWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::LoadWindow) {
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
}

LoadWindow::~LoadWindow() {
    delete ui;
}
