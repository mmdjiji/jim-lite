#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QWebSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_connect_clicked();
    void on_send_clicked();
    void onConnected();
    void onTextMessageReceived(QString);


private:
    Ui::MainWindow *ui;
    QWebSocket *socket;
};

#endif // MAINWINDOW_H
