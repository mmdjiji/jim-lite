#ifndef SIGNUPWINDOW_H
#define SIGNUPWINDOW_H

#include <QMainWindow>
#include <QMessageBox>

#include "protocol.h"
#include "facesignupwindow.h"

namespace Ui {
class SignUpWindow;
}

class SignUpWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SignUpWindow(QWidget *parent = 0);
    ~SignUpWindow();
    void setProtocol(Protocol *protocol);

private slots:
    void on_signUp_clicked();
    void signUpSuccess();
    void signUpFailed();
    void on_addFace_clicked();

private:
    Ui::SignUpWindow *ui;
    Protocol *protocol;
    FaceSignUpWindow *faceSignUpWinodw = nullptr;
};

#endif // SIGNUPWINDOW_H
