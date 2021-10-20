#include "signupwindow.h"
#include "ui_signupwindow.h"

SignUpWindow::SignUpWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::SignUpWindow) {
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() &~ Qt::WindowMaximizeButtonHint);
}

SignUpWindow::~SignUpWindow() {
    delete ui;
}

void SignUpWindow::setProtocol(Protocol *protocol) {
    this->protocol = protocol;
    connect(protocol, SIGNAL(signUpSuccess()), this, SLOT(signUpSuccess()));
    connect(protocol, SIGNAL(signUpFailed()), this, SLOT(signUpFailed()));
    connect(ui->nickname, SIGNAL(returnPressed()), this, SLOT(on_signUp_clicked()));
}

void SignUpWindow::on_signUp_clicked() {
    if(ui->username->text() != "" && ui->password->text() != "" && ui->nickname->text() != "") {
        protocol->sendSignUp(ui->username->text(), ui->password->text(), ui->nickname->text());
    } else {
        QMessageBox::critical(NULL, "JIM-Lite", "请输入用户名、密码和昵称!");
    }
}

void SignUpWindow::signUpSuccess() {
    QMessageBox::information(NULL, "JIM-Lite", "注册成功，请返回登录！");
    ui->username->setText("");
    ui->password->setText("");
    ui->nickname->setText("");
    this->close();
}

void SignUpWindow::signUpFailed() {
    QMessageBox::critical(NULL, "JIM-Lite", "注册失败，用户名重复或不合法。");
}

void SignUpWindow::on_addFace_clicked() {
    if(!faceSignUpWinodw) {
        faceSignUpWinodw = new FaceSignUpWindow(this);
    }
    faceSignUpWinodw->initCamera();
    faceSignUpWinodw->show();
}
