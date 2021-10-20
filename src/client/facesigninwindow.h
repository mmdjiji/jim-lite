#ifndef FACESIGNINWINDOW_H
#define FACESIGNINWINDOW_H

#include <QMainWindow>
#include <QCamera>
#include <QCameraInfo>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QPushButton>
#include <QFile>
#include <QEvent>
#include "protocol.h"

namespace Ui {
class FaceSignInWindow;
}

class FaceSignInWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit FaceSignInWindow(QWidget *parent = 0);
    void setProtocol(Protocol *protocol);
    void initCamera();
    ~FaceSignInWindow();

private:
    Ui::FaceSignInWindow *ui;
    QCamera* myCamera;
    QCamera* camera;
    QCameraViewfinder* vf;
    QCameraImageCapture* cp;
    Protocol *protocol = nullptr;

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void savePicture(int id, const QImage &preview);
    void on_pushButton_clicked();
};

#endif // FACESIGNINWINDOW_H
