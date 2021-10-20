#include "protocol.h"

Protocol::Protocol() {
    this->socket = new QWebSocket();
}

Protocol::~Protocol() {
    delete this->socket;
}

bool Protocol::connectServer(QString ws) {
    this->socket->open(QUrl(ws));
    connect(this->socket, SIGNAL(connected()), this, SLOT(onConnected()));
    return true;
}

void Protocol::onConnected() {
    connect(this->socket, SIGNAL(textMessageReceived(QString)), this, SLOT(onTextMessageReceived(QString)));
}


void Protocol::onTextMessageReceived(QString data) {
    qDebug() << data;
    this->router(data);
}

bool Protocol::sendSignIn(QString username, QString password) { // send 01
    if(username != "" && password != "") {
        QJsonObject res;
        res.insert("package", "signIn");
        res.insert("username", username);
        res.insert("password", password);
        this->me.username = username;
        this->me.password = password;
        QJsonDocument resDoc(res);
        qDebug() << resDoc.toJson();
        socket->sendTextMessage(resDoc.toJson());
        return true;
    }
    return false;
}

bool Protocol::sendSignUp(QString username, QString password, QString nickname) { // send 02
    if(username != "" && password != "" && nickname != "") {
        QFile file("signUp.jpg");

        QJsonObject res;
        res.insert("package", "signUp");
        res.insert("username", username);
        res.insert("password", password);
        res.insert("nickname", nickname);
        if(file.exists()) {
            file.open(QFile::ReadOnly);
            res.insert("face", QString(file.readAll().toBase64()));
            file.close();
            file.remove();
        }
        QJsonDocument resDoc(res);
        qDebug() << resDoc.toJson();
        socket->sendTextMessage(resDoc.toJson());
        return true;
    }
    return false;
}

bool Protocol::sendGetFriendsList() { // send 03
    QJsonObject res;
    res.insert("package", "getFriendsList");
    res.insert("token", this->token);
    QJsonDocument resDoc(res);
    qDebug() << resDoc.toJson();
    socket->sendTextMessage(resDoc.toJson());
    return true;
}

bool Protocol::sendFriendMessage(Message message) { // send 06
    if(message.text != "") {
        QJsonObject msg;
        msg.insert("dst", message.dst);
        msg.insert("type", message.type);
        msg.insert("text", message.text);
        QJsonObject res;
        res.insert("package", "sendFriendMessage");
        res.insert("token", this->token);
        res.insert("message", msg);
        QJsonDocument resDoc(res);
        qDebug() << resDoc.toJson();
        socket->sendTextMessage(resDoc.toJson());
        return true;
    }
    return false;
}

bool Protocol::sendFaceSignIn(QString face_base64) { // send 13
    if(face_base64 != "") {
        QJsonObject res;
        res.insert("package", "faceSignIn");
        res.insert("face", face_base64);
        QJsonDocument resDoc(res);
        qDebug() << resDoc.toJson();
        socket->sendTextMessage(resDoc.toJson());
        return true;
    }
    return false;
}


void Protocol::router(QString data) {
    QJsonDocument reqDoc = QJsonDocument::fromJson(data.toUtf8());
    if(reqDoc.isObject()) {
        QJsonObject req = reqDoc.object();
        QString action = req.value("package").toString();
        if(action == "signIn") { // 01
            recvSignIn(data);
        } else if(action == "signUp") { // 02
            recvSignUp(data);
        } else if(action == "getFriendsList") { // 03
            recvGetFriendsList(data);
        } else if(action == "recvFriendMessage") { // 08
            recvFriendMessage(data);
        }
    }
}


void Protocol::recvSignIn(QString data) { // recv 01
    QJsonDocument reqDoc = QJsonDocument::fromJson(data.toUtf8());
    if(reqDoc.isObject()) {
        QJsonObject req = reqDoc.object();
        QString result = req.value("result").toString();
        if(result == "success") {
            QString username = req.value("username").toString();
            QString nickname = req.value("nickname").toString();
            QString token = req.value("token").toString();

            this->me.username = username;
            this->me.nickname = nickname;
            this->token = token;
            emit signInSuccess();
        } else {
            emit signInFailed();
        }
    }
}


void Protocol::recvSignUp(QString data) { // recv 02
    QJsonDocument reqDoc = QJsonDocument::fromJson(data.toUtf8());
    if(reqDoc.isObject()) {
        QJsonObject req = reqDoc.object();
        QString result = req.value("result").toString();
        if(result == "success") {
            emit signUpSuccess();
        } else {
            emit signUpFailed();
        }
    }
}

void Protocol::recvGetFriendsList(QString data) { // recv 03
    QJsonDocument reqDoc = QJsonDocument::fromJson(data.toUtf8());
    if(reqDoc.isObject()) {
        QJsonObject req = reqDoc.object();
        QString result = req.value("result").toString();
        if(result == "success") {
            QJsonArray friends = req.value("friends").toArray();
            this->friendsUsername.clear();
            this->friendsNickname.clear();
            for(int i=0; i<friends.count(); ++i) {
                this->friendsUsername.push_back(friends.at(i).toObject().value("username").toString());
                this->friendsNickname[friends.at(i).toObject().value("username").toString()] = friends.at(i).toObject().value("nickname").toString();
            }
            emit getFriendsListSuccess();
        }
    }
}

void Protocol::recvFriendMessage(QString data) { // recv 08
    QJsonDocument reqDoc = QJsonDocument::fromJson(data.toUtf8());
    if(reqDoc.isObject()) {
        QJsonObject req = reqDoc.object();
        QJsonObject message = req.value("message").toObject();

        QString time = message.value("time").toString();
        QString src = message.value("src").toString();
        QString type = message.value("type").toString();
        QString text = message.value("text").toString();
        Message msg;
        msg.time = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss");
        msg.src = src;
        msg.dst = this->me.username;
        msg.type = type;
        msg.text = text;
        record.insertRecordByUsername(me.username, msg.src, msg);
        if(openWindowWith != msg.src) {
            ++friendsNewMessage[msg.src];
        }
        emit needRefreshFriendsList();
        emit needRefreshMessage();
        qDebug() << "收到消息: " << msg.text;
    }
}
