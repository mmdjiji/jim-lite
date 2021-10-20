#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QWebSocket>
#include <QObject>
#include <QVariant>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

#include <vector>
#include <map>

#include "user.h"
#include "message.h"
#include "record.h"

class Protocol : public QObject {
    Q_OBJECT
public:
    Protocol();
    ~Protocol();
    User me;
    std::vector<QString> friendsUsername;
    std::map<QString, QString> friendsNickname;
    std::map<QString, int> friendsNewMessage;
    QString openWindowWith;

    bool connectServer(QString ws);
    bool sendSignIn(QString username, QString password);
    bool sendSignUp(QString username, QString password, QString nickname);
    bool sendGetFriendsList();
    bool sendFriendMessage(Message message);
    bool sendFaceSignIn(QString face_base64);

private:
    QWebSocket *socket;
    QString token;
    Record record;

    void router(QString data); // router
    void recvSignIn(QString data); // package 01
    void recvSignUp(QString data); // package 02
    void recvGetFriendsList(QString data); // package 03
    void recvFriendMessage(QString data); // package 08

signals:
    // signIn
    void signInSuccess();
    void signInFailed();

    // signUp
    void signUpSuccess();
    void signUpFailed();

    // getFriendsList
    void getFriendsListSuccess();

    // refresh for TalkWindow
    void needRefreshMessage();

    // refresh for MainWindow
    void needRefreshFriendsList();

private slots:
    void onConnected();
    void onTextMessageReceived(QString data);
};

#endif // PROTOCOL_H
