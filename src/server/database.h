#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#include <QString>
#include <QDebug>

#include <vector>

#include "user.h"

class Database {
public:
    Database();
    ~Database();
    std::vector<User> selectUser();
    User *selectUserByUsername(QString username);
    bool insertUser(User user);
private:
    QSqlDatabase db;
};

#endif // DATABASE_H
