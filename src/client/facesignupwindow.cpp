#include "facesignupwindow.h"
#include "ui_facesignupwindow.h"

FaceSignUpWindow::FaceSignUpWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::FaceSignUpWindow) {
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() &~ Qt::WindowMaximizeButtonHint);
}

FaceSignUpWindow::~FaceSignUpWindow() {
    delete ui;
}

void FaceSignUpWindow::initCamera() {
    myCamera = new QCamera(this); // 创建一个获取图片对象
    cp = new QCameraImageCapture(myCamera); // 抓取图片的信号
    connect(cp, &QCameraImageCapture::imageCaptured, this, &FaceSignUpWindow::savePicture);
    vf = new QCameraViewfinder(ui->widget);
    vf->resize(ui->widget->size());
    myCamera->setViewfinder(vf);
    vf->show();
    myCamera->start();
}

void FaceSignUpWindow::savePicture(int id, const QImage &preview) {
    QFile file("signUp.jpg");
    file.open(QFile::WriteOnly);
    preview.save(&file, "JPG");
    file.close();
}

void FaceSignUpWindow::on_pushButton_clicked() {
    cp->capture();
}

void FaceSignUpWindow::closeEvent(QCloseEvent *event) {
    myCamera->stop();
    vf->close();
}
