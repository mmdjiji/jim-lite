#ifndef SIGNINWINDOW_H
#define SIGNINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

#include "mainwindow.h"
#include "signupwindow.h"
#include "facesigninwindow.h"
#include "protocol.h"

namespace Ui {
class SignInWindow;
}

class SignInWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SignInWindow(QWidget *parent = 0);
    ~SignInWindow();

private:
    Ui::SignInWindow *ui;
    Protocol *protocol = nullptr;

    MainWindow *mainWindow = nullptr;
    SignUpWindow *signUpWindow = nullptr;
    FaceSignInWindow *faceSignInWindow = nullptr;

private slots:
    void on_signIn_clicked();
    void on_signUp_clicked();
    void signInSuccess();
    void signInFailed();
    void on_faceSignIn_clicked();
};

#endif // SIGNINWINDOW_H
