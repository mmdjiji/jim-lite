#include "signinwindow.h"
#include "ui_signinwindow.h"

SignInWindow::SignInWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::SignInWindow) {
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() &~ Qt::WindowMaximizeButtonHint);
    protocol = new Protocol();
    protocol->connectServer("ws://127.0.0.1:3000");
    connect(protocol, SIGNAL(signInSuccess()), this, SLOT(signInSuccess()));
    connect(protocol, SIGNAL(signInFailed()), this, SLOT(signInFailed()));

    connect(ui->password, SIGNAL(returnPressed()), this, SLOT(on_signIn_clicked()));
}

SignInWindow::~SignInWindow() {
    delete protocol;
    delete ui;
}

void SignInWindow::on_signIn_clicked() {
    if(ui->username->text() != "" && ui->password->text() != "") {
        protocol->sendSignIn(ui->username->text(), ui->password->text());
    } else {
        QMessageBox::critical(NULL, "JIM-Lite", "请输入用户名和密码!");
    }
}

void SignInWindow::on_signUp_clicked() {
    if(!signUpWindow) {
        signUpWindow = new SignUpWindow(this);
        signUpWindow->setProtocol(protocol);
    }
    signUpWindow->show();
}

void SignInWindow::signInSuccess() {
    if(faceSignInWindow && faceSignInWindow->isVisible()) {
        faceSignInWindow->close();
    }
    this->close();
    if(!mainWindow) {
        mainWindow = new MainWindow();
        mainWindow->setProtocol(protocol);
    }
    mainWindow->show();
}

void SignInWindow::signInFailed() {
    QMessageBox::critical(NULL, "JIM-Lite", "登录失败");
}

void SignInWindow::on_faceSignIn_clicked() {
    if(!faceSignInWindow) {
        faceSignInWindow = new FaceSignInWindow(this);
        faceSignInWindow->setProtocol(protocol);
    }
    faceSignInWindow->initCamera();
    faceSignInWindow->show();
}
