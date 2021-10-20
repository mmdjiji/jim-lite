#ifndef RECORD_H
#define RECORD_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

#include <vector>

#include "message.h"

class Record {
public:
    Record();
    ~Record();
    std::vector<Message> selectRecordByUsername(QString myName, QString username);
    bool insertRecordByUsername(QString myName, QString username, Message message);

private:
    QSqlDatabase db;
    bool createTableIfNotExists(QString myName, QString username);

};

#endif // RECORD_H
