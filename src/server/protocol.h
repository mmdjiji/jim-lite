#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QObject>
#include <QVAriant>
#include <QWebSocketServer>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>

#include <vector>

#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include "user.h"
#include "message.h"
#include "database.h"

class Protocol: public QObject {
    Q_OBJECT
public:
    Protocol();
    ~Protocol();
    // Utils
    void logger(QString value);

    // For socket
    bool listen(int port);

private:
    Database db;
    QWebSocketServer *server = nullptr;
    std::vector<QWebSocket *> socket;
    std::vector<Message> messageQueue;

    // Message Queue
    void sendMessageQueue(QWebSocket *theSocket);

    // Parse message
    void router(QWebSocket *theSocket, QString data); // router
    void signIn(QWebSocket *theSocket, QString data); // package 01
    bool isValid(QString text);
    void addFace(QString username, QString face_base64);
    void signUp(QWebSocket *theSocket, QString data); // package 02
    void getFriendsList(QWebSocket *theSocket, QString data); // package 03
    void getGroupsList(QWebSocket *theSocket, QString data); // package 04

    void sendFriendMessage(QWebSocket *theSocket, QString data); // package 06

    void faceSignIn(QWebSocket *theSocket, QString data); // package 13

private slots:
    void onNewConnection();
    void onTextMessageReceived(QString);
    void onDisconnected();
    void onRequestFinished(QNetworkReply *reply);

signals:
   void sig_logger(QString value);

};

#endif // PROTOCOL_H
