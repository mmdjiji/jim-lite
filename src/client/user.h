#ifndef USER_H
#define USER_H

#include <QString>

class User {
public:
    User(QString username = "", QString password = "", QString nickname = "", QString groupslist = "[]");
    QString username;
    QString password;
    QString nickname;
    QString groupslist;
};

#endif // USER_H
