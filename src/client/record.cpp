#include "record.h"

Record::Record() {
    if(QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }
    db.setDatabaseName("record.db");

    if (!db.open()) {
        qDebug() << "Error: Failed to connect database." << db.lastError();
    } else {
        qDebug() << "Succeed to connect database.";
    }
}

Record::~Record() {
    db.close();
}

bool Record::createTableIfNotExists(QString myName, QString username) {
    QSqlQuery query;
    query.prepare(QString("create table if not exists rec_%1_with_%2 (time varchar, src varchar, dst varchar, type varchar, text varchar);").arg(myName).arg(username));
    if(!query.exec()) {
        qDebug() << "Error: Fail to create table." << query.lastError();
        query.finish();
        return false;
    } else {
        qDebug() << "Record table OK!";
        query.finish();
        return true;
    }
}

std::vector<Message> Record::selectRecordByUsername(QString myName, QString username) {
    std::vector<Message> retval;
    if(createTableIfNotExists(myName, username)) {
        QSqlQuery query;
        query.prepare(QString("select * from rec_%1_with_%2").arg(myName).arg(username));
        if(query.exec()) {
            while(query.next()) {
                Message message;
                message.time = QDateTime::fromString(query.value(0).toString(), "yyyy-MM-dd hh:mm:ss");
                message.src = query.value(1).toString();
                message.dst = query.value(2).toString();
                message.type = query.value(3).toString();
                message.text = query.value(4).toString();
                retval.push_back(message);
            }
        } else {
            qDebug() << "Error: Fail to select user." << query.lastError();
        }
        query.finish();
    }
    return retval;
}

bool Record::insertRecordByUsername(QString myName, QString username, Message message) {
    if(createTableIfNotExists(myName, username)) {
        QSqlQuery query;
        query.prepare(QString("insert into rec_%1_with_%2 (time, src, dst, type, text) values (:time, :src, :dst, :type, :text);").arg(myName).arg(username));
        query.bindValue(":time", message.time.toString("yyyy-MM-dd hh:mm:ss"));
        query.bindValue(":src", message.src);
        query.bindValue(":dst", message.dst);
        query.bindValue(":type", message.type);
        query.bindValue(":text", message.text);
        if(query.exec()) {
            query.finish();
            return true;
        }
        qDebug() << "Error: Fail to insert user." << query.lastError();
        query.finish();
    }
    return false;
}
