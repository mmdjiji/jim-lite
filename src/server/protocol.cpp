#include "protocol.h"

Protocol::Protocol() {
    this->server = new QWebSocketServer("server", QWebSocketServer::NonSecureMode);
}

Protocol::~Protocol() {
    if(this->server) delete this->server;
    for(std::vector<QWebSocket *>::iterator it = this->socket.begin(); it != this->socket.end();) {
        // if(*it) delete *it; // [Debug] Maybe null pointer exception?
        this->socket.erase(it);
    }
}

void Protocol::logger(QString value) {
    emit sig_logger(value);
}

bool Protocol::listen(int port) {
    if(!this->server->listen(QHostAddress::Any, port)) {
        logger(QString("[Error] 监听 %1 端口失败，请检查是否已被占用。").arg(port));
        return false;
    }
    logger(QString("[Info] 开始监听 %1 端口...").arg(port));
    connect(this->server, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
    return true;
}

void Protocol::onNewConnection() {
    QWebSocket *newSocket = server->nextPendingConnection();

    logger(QString("[Info] 有新主机 {%1:%2} 连接。").arg(newSocket->peerAddress().toString()).arg(newSocket->peerPort()));

    connect(newSocket, SIGNAL(textMessageReceived(QString)), this, SLOT(onTextMessageReceived(QString)));
    connect(newSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

    this->socket.push_back(newSocket);
}

void Protocol::onTextMessageReceived(QString data) {
    QWebSocket *theSocket = (QWebSocket *) sender();
    // logger(QString("[Info] {%1:%2}: '%3'。").arg(theSocket->peerAddress().toString()).arg(theSocket->peerPort()).arg(data));
    this->router(theSocket, data);
}

void Protocol::onDisconnected() {
    QWebSocket *oldSocket = (QWebSocket *) sender();
    logger(QString("[Info] 主机 {%1:%2} 断开连接。").arg(oldSocket->peerAddress().toString()).arg(oldSocket->peerPort()));
    for(std::vector<QWebSocket *>::iterator it = this->socket.begin(); it != this->socket.end(); ++it) {
        if(oldSocket == *it) {
            this->socket.erase(it);
            break;
        }
    }
    // if(oldSocket) delete oldSocket; // [Debug] Maybe null pointer exception?
}


void Protocol::sendMessageQueue(QWebSocket *theSocket) {
    for(std::vector<Message>::iterator it = messageQueue.begin(); it != messageQueue.end(); ++it) {
        if(it->dst == theSocket->property("username").toString()) {
            // Make message 08
            QJsonObject msg;
            msg.insert("time", it->time.toString("yyyy-MM-dd hh:mm:ss"));
            msg.insert("src", it->src);
            msg.insert("type", it->type);
            msg.insert("text", it->text);

            QJsonObject send;
            send.insert("package", "recvFriendMessage");
            send.insert("message", msg);

            QJsonDocument sendDoc(send);
            qDebug() << sendDoc.toJson();
            theSocket->sendTextMessage(QString(sendDoc.toJson()));
            messageQueue.erase(it);
            --it;
        }
    }
}

void Protocol::router(QWebSocket *theSocket, QString data) {
    QJsonDocument reqDoc = QJsonDocument::fromJson(data.toUtf8());
    if(reqDoc.isObject()) {
        QJsonObject req = reqDoc.object();
        QString action = req.value("package").toString();
        if(action == "signIn") { // 01
            signIn(theSocket, data);
        } else if(action == "signUp") { // 02
            signUp(theSocket, data);
        } else if(action == "getFriendsList") { // 03
            getFriendsList(theSocket, data);
        } else if(action == "getGroupsList") {
            getGroupsList(theSocket, data);
        } else if(action == "sendFriendMessage") { // 06
            sendFriendMessage(theSocket, data);
        } else if(action == "faceSignIn") { // 13
            faceSignIn(theSocket, data);
        }
    }
}

void Protocol::signIn(QWebSocket *theSocket, QString data) { // 01
    QJsonDocument reqDoc = QJsonDocument::fromJson(data.toUtf8());
    if(reqDoc.isObject()) {
        QJsonObject req = reqDoc.object();
        QString username = req.value("username").toString();
        QString password = req.value("password").toString();

        // try to login
        User *result = db.selectUserByUsername(username);

        QJsonObject res;
        bool success = false;
        res.insert("package", "signIn");
        if(result) {
            if(result->password == password) { // sign in success
                success = true;
                QUuid token = QUuid::createUuid();
                theSocket->setProperty("username", username); // Set property for socket
                theSocket->setProperty("token", token.toString());
                res.insert("result", "success");
                res.insert("username", username);
                res.insert("nickname", result->nickname);
                res.insert("token", token.toString());
            }
            delete result;
        }
        if(!success) { // sign in failed
            res.insert("result", "failed");
            res.insert("reason", "Invalid username or password.");
        }
        QJsonDocument resDoc(res);
        qDebug() << resDoc.toJson();
        theSocket->sendTextMessage(resDoc.toJson());
    }
}

bool Protocol::isValid(QString text) {
    QByteArray textArr = text.toUtf8();
    char *arr = textArr.data();
    while(*arr) {
        if((*arr >= 'A' && *arr <= 'Z') || (*arr >= 'a' && *arr <= 'z') ||(*arr >='0' && *arr <= '9')) {
            ++arr;
        } else {
            return false;
        }
    }
    return true;
}

void Protocol::addFace(QString username, QString face_base64) {
    QNetworkRequest request;
    QNetworkAccessManager* networkManager = new QNetworkAccessManager(this);
    request.setUrl(QUrl("http://127.0.0.1:4000/"));
    request.setRawHeader("accept", "*/*");
    request.setRawHeader("Content-Type", "text/plain;charset=UTF-8");
    QString body = "{\"package\": \"signUp\", \"username\": \"%1\", \"face\": \"data:image/jpg;base64,%2\"}";
    QNetworkReply *reply = networkManager->post(request, body.arg(username).arg(face_base64).toUtf8());
}

void Protocol::signUp(QWebSocket *theSocket, QString data) { // 02
    QJsonDocument reqDoc = QJsonDocument::fromJson(data.toUtf8());
    if(reqDoc.isObject()) {
        QJsonObject req = reqDoc.object();

        QString username = req.value("username").toString();
        QString password = req.value("password").toString();
        QString nickname = req.value("nickname").toString();
        QString face = req.value("face").toString();

        QJsonObject res;
        res.insert("package", "signUp");

        if(username != "" && password != "" && nickname != "" && isValid(username)) {
            User newUser(username, password, nickname);

            // try to register
            bool result = db.insertUser(newUser);

            if(result) { // sign up success
                // send sign up information to all users
                for(std::vector<QWebSocket *>::iterator it = socket.begin(); it != socket.end(); ++it) {
                        std::vector<User> users = db.selectUser();
                        QJsonArray friends;
                        for(std::vector<User>::iterator it_inner = users.begin(); it_inner != users.end(); ++it_inner) {
                            if(it_inner->username != (*it)->property("username")) {
                                QJsonObject member;
                                member.insert("username", it_inner->username);
                                member.insert("nickname", it_inner->nickname);
                                friends.append(member);
                            }
                        }
                        QJsonObject send;
                        send.insert("package", "getFriendsList");
                        send.insert("result", "success");
                        send.insert("friends", friends);
                        if(face != "") {
                            addFace(username, face);
                        }
                        QJsonDocument sendDoc(send);
                        qDebug() << sendDoc.toJson();
                        (*it)->sendTextMessage(sendDoc.toJson());
                }
                res.insert("result", "success");
            } else { // sign up failed
                res.insert("result", "failed");
                res.insert("reason", "Duplicate user.");
            }
        } else {
            res.insert("result", "failed");
            res.insert("reason", "Invalid sign up user.");
        }
        QJsonDocument resDoc(res);
        qDebug() << resDoc.toJson();
        theSocket->sendTextMessage(resDoc.toJson());
    }
}

void Protocol::getFriendsList(QWebSocket *theSocket, QString data) { // 03
    QJsonDocument reqDoc = QJsonDocument::fromJson(data.toUtf8());
    if(reqDoc.isObject()) {
        QJsonObject req = reqDoc.object();
        QString token = req.value("token").toString();
        QJsonObject res;
        res.insert("package", "getFriendsList");
        if(theSocket->property("token").toString() == token) {
            std::vector<User> users = db.selectUser();
            QJsonArray friends;
            for(std::vector<User>::iterator it = users.begin(); it != users.end(); ++it) {
                if(it->username != theSocket->property("username")) {
                    QJsonObject member;
                    member.insert("username", it->username);
                    member.insert("nickname", it->nickname);
                    friends.append(member);
                }
            }
            res.insert("result", "success");
            res.insert("friends", friends);
        } else {
            res.insert("result", "failed");
            res.insert("reason", "Invalid token.");
        }
        QJsonDocument resDoc(res);
        qDebug() << resDoc.toJson();
        theSocket->sendTextMessage(resDoc.toJson());
        sendMessageQueue(theSocket);
    }
}

void Protocol::getGroupsList(QWebSocket *theSocket, QString data) { // 04
    QJsonDocument reqDoc = QJsonDocument::fromJson(data.toUtf8());
    if(reqDoc.isObject()) {
        QJsonObject req = reqDoc.object();
        QString token = req.value("token").toString();
        QJsonObject res;
        res.insert("package", "getGroupsList");
        if(theSocket->property("token").toString() == token) {
            User* user = db.selectUserByUsername(theSocket->property("username").toString());
            if(user) {
                res.insert("groups", user->groupslist);
                res.insert("result", "success");
                delete user;
            } else {
                res.insert("result", "failed");
                res.insert("reason", "A protocol bug occured.");
            }
        } else {
            res.insert("result", "failed");
            res.insert("reason", "Invalid token.");
        }
        QJsonDocument resDoc(res);
        qDebug() << resDoc.toJson();
        theSocket->sendTextMessage(resDoc.toJson());
        sendMessageQueue(theSocket);
    }
}

void Protocol::sendFriendMessage(QWebSocket *theSocket, QString data) { // 06
    QJsonDocument reqDoc = QJsonDocument::fromJson(data.toUtf8());
    if(reqDoc.isObject()) {
        QJsonObject req = reqDoc.object();
        QString token = req.value("token").toString();
        QJsonObject res;
        res.insert("package", "sendFriendMessage");
        if(theSocket->property("token").toString() == token) {
            QJsonObject msgObj = req.value("message").toObject();

            // online => send, else append to messageQueue
            Message message;
            message.src = theSocket->property("username").toString();
            message.dst = msgObj.value("dst").toString();
            message.type = msgObj.value("type").toString();
            message.text = msgObj.value("text").toString();
            message.time = QDateTime::currentDateTime();

            // forward if and only if the dst user in the database
            User *dstUser = db.selectUserByUsername(message.dst);
            if(dstUser) {

                bool flag = false;
                for(std::vector<QWebSocket *>::iterator it = socket.begin(); it != socket.end(); ++it) {
                    if(message.dst == (*it)->property("username")) {

                        // Make message 08
                        QJsonObject msg;
                        msg.insert("time", message.time.toString("yyyy-MM-dd hh:mm:ss"));
                        msg.insert("src", message.src);
                        msg.insert("type", message.type);
                        msg.insert("text", message.text);

                        QJsonObject send;
                        send.insert("package", "recvFriendMessage");
                        send.insert("message", msg);

                        QJsonDocument sendDoc(send);
                        qDebug() << sendDoc.toJson();
                        (*it)->sendTextMessage(QString(sendDoc.toJson()));
                        qDebug() << "Message forwarded.";

                        flag = true;
                        break;
                    }
                }
                if(!flag) {
                    qDebug() << "Add to MessageQueue.";
                    messageQueue.push_back(message);
                }
                res.insert("result", "success");
                delete dstUser;
            } else {
                res.insert("result", "failed");
                res.insert("reason", "Invalid username.");
            }
        } else {
            res.insert("result", "failed");
            res.insert("reason", "Invalid token.");
        }
        QJsonDocument resDoc(res);
        qDebug() << resDoc.toJson();
        theSocket->sendTextMessage(resDoc.toJson());
    }
}

void Protocol::faceSignIn(QWebSocket *theSocket, QString data) { // 13
    QJsonDocument reqDoc = QJsonDocument::fromJson(data.toUtf8());
    if(reqDoc.isObject()) {
        QJsonObject req = reqDoc.object();
        QString face = req.value("face").toString();

        QNetworkRequest request;
        QNetworkAccessManager* networkManager = new QNetworkAccessManager(this);
        QMetaObject::Connection connRet = QObject::connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onRequestFinished(QNetworkReply*)));
        Q_ASSERT(connRet);
        request.setUrl(QUrl("http://127.0.0.1:4000/"));
        request.setRawHeader("accept", "*/*");
        request.setRawHeader("Content-Type", "text/plain;charset=UTF-8");
        QString body = "{\"package\": \"signIn\", \"face\": \"data:image/jpg;base64,%1\"}";
        QNetworkReply *reply = networkManager->post(request, body.arg(face).toUtf8());
        for(std::vector<QWebSocket *>::iterator it = socket.begin(); it != socket.end(); ++it) {
            if(*it == theSocket) {
                networkManager->setProperty("number", it - socket.begin());
                break;
            }
        }
        qDebug() << "posted";
    }
}

void Protocol::onRequestFinished(QNetworkReply *reply) {
    QWebSocket *theSocket = socket.at(sender()->property("number").toInt());
    QVariant statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    if(statusCode.isValid()) {
        //qDebug() << "status code=" << statusCode.toInt();
    }
    QVariant reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
    if(reason.isValid()) {
        //qDebug() << "reason=" << reason.toString();
    }
    QNetworkReply::NetworkError err = reply->error();
    if(err != QNetworkReply::NoError) {
        //qDebug() << "Failed: " << reply->errorString();
    } else {
        QByteArray data = reply->readAll();
        qDebug() << data;
        if(data != "") {
            QJsonDocument reqDoc = QJsonDocument::fromJson(data);
            QJsonObject req = reqDoc.object().value("bestMatch").toObject();
            QString label = req.value("_label").toString();
            double distance = req.value("_distance").toDouble();

            qDebug() << distance;
            if(distance < 0.3) {
                //登录成功
                qDebug() << "success";
                User *result = db.selectUserByUsername(label);
                if(result) {
                    qDebug() << "foundUser";
                    QJsonObject res;
                    res.insert("package", "signIn");
                    QUuid token = QUuid::createUuid();
                    theSocket->setProperty("username", result->username); // Set property for socket
                    theSocket->setProperty("token", token.toString());
                    res.insert("result", "success");
                    res.insert("username", result->username);
                    res.insert("nickname", result->nickname);
                    res.insert("token", token.toString());

                    QJsonDocument resDoc(res);
                    qDebug() << resDoc.toJson();
                    theSocket->sendTextMessage(resDoc.toJson());
                    delete result;
                    return;
                }
            }
        }
    }
    QJsonObject res;
    res.insert("package", "signIn");
    res.insert("result", "failed");
    res.insert("reason", "Invalid face.");
    QJsonDocument resDoc(res);
    qDebug() << resDoc.toJson();
    theSocket->sendTextMessage(resDoc.toJson());
}
