#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>

#include "talkwindow.h"
#include "protocol.h"
#include "user.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void setProtocol(Protocol *protocol);
    void refreshFriendsList();

private slots:
    void getFriendsListSuccess();
    void friendClicked();

private:
    Ui::MainWindow *ui;
    Protocol *protocol = nullptr;
    TalkWindow *talkWindow = nullptr;
};

#endif // MAINWINDOW_H
