#include "user.h"

User::User(QString username, QString password, QString nickname, QString groupslist)
{
    this->username = username;
    this->password = password;
    this->nickname = nickname;
    this->groupslist = groupslist;
}
