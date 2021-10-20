#include "facesigninwindow.h"
#include "ui_facesigninwindow.h"

FaceSignInWindow::FaceSignInWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::FaceSignInWindow) {
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() &~ Qt::WindowMaximizeButtonHint);
}

FaceSignInWindow::~FaceSignInWindow() {
    delete ui;
}

void FaceSignInWindow::setProtocol(Protocol *protocol) {
    this->protocol = protocol;
}

void FaceSignInWindow::initCamera() {
    myCamera = new QCamera(this); // 创建一个获取图片对象
    cp = new QCameraImageCapture(myCamera); // 抓取图片的信号
    connect(cp, &QCameraImageCapture::imageCaptured, this, &FaceSignInWindow::savePicture);
    vf = new QCameraViewfinder(ui->widget);
    vf->resize(ui->widget->size());
    myCamera->setViewfinder(vf);
    vf->show();
    myCamera->start();
}


void FaceSignInWindow::savePicture(int id, const QImage &preview) {
    QFile file("tmp.jpg");
    file.open(QFile::WriteOnly);
    preview.save(&file, "JPG");
    file.close();
    file.open(QFile::ReadOnly);
    QByteArray fileValue = file.readAll();
    protocol->sendFaceSignIn(QString(fileValue.toBase64()));
    file.close();
    file.remove();
}

void FaceSignInWindow::on_pushButton_clicked() {
    cp->capture();
}


void FaceSignInWindow::closeEvent(QCloseEvent *event) {
    myCamera->stop();
    vf->close();
}
