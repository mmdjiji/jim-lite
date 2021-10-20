#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setWindowFlags(this->windowFlags() &~ Qt::WindowMaximizeButtonHint);
}

void MainWindow::setProtocol(Protocol *protocol) {
    this->protocol = protocol;
    connect(this->protocol, SIGNAL(getFriendsListSuccess()), this, SLOT(getFriendsListSuccess()));
    connect(this->protocol, SIGNAL(needRefreshFriendsList()), this, SLOT(getFriendsListSuccess()));
    this->protocol->sendGetFriendsList();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::refreshFriendsList() {
        QWidget *widget = ui->list->widget(0);
        if(widget->layout()) {
            delete widget->layout();
        }
        QVBoxLayout *layout = new QVBoxLayout();

        for(std::vector<QString>::iterator it = protocol->friendsUsername.begin(); it != protocol->friendsUsername.end(); ++it) {
            QString format;
            if(protocol->friendsNewMessage[*it] > 0) {
                format = QString("%1 (%2)").arg(*it).arg(protocol->friendsNewMessage[*it]);
            } else {
                format = QString("%1").arg(*it);
            }
            QPushButton *button = new QPushButton(format);
            button->setProperty("username", *it);
            button->setProperty("nickname", protocol->friendsNickname[*it]);
            layout->addWidget(button, 0, Qt::AlignTop);
            connect(button, SIGNAL(clicked(bool)), this, SLOT(friendClicked()));
        }
        layout->addStretch(50);
        widget->setLayout(layout);
}

void MainWindow::getFriendsListSuccess() {
    this->refreshFriendsList();
}

void MainWindow::friendClicked() {
    if(!talkWindow) {
        talkWindow = new TalkWindow(this);
        talkWindow->setProtocol(protocol);
    }
    protocol->openWindowWith = sender()->property("username").toString();
    protocol->friendsNewMessage[sender()->property("username").toString()] = 0;
    this->refreshFriendsList();
    talkWindow->setOthers(false, sender()->property("username").toString(), sender()->property("nickname").toString());
    talkWindow->show();
}
