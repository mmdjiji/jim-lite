#ifndef FACESIGNUPWINDOW_H
#define FACESIGNUPWINDOW_H

#include <QMainWindow>
#include <QCamera>
#include <QCameraInfo>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include <QPushButton>
#include <QFile>
#include <QEvent>

namespace Ui {
class FaceSignUpWindow;
}

class FaceSignUpWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit FaceSignUpWindow(QWidget *parent = 0);
    void initCamera();
    ~FaceSignUpWindow();

private:
    Ui::FaceSignUpWindow *ui;
    QCamera* myCamera;
    QCamera* camera;
    QCameraViewfinder* vf;
    QCameraImageCapture* cp;

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void savePicture(int id, const QImage &preview);
    void on_pushButton_clicked();
};

#endif // FACESIGNUPWINDOW_H
