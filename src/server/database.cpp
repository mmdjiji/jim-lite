#include "database.h"

Database::Database() {
    if(QSqlDatabase::contains("qt_sql_default_connection")) {
        db = QSqlDatabase::database("qt_sql_default_connection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE");
    }
    db.setDatabaseName("server.db");

    if (!db.open()) {
        qDebug() << "Error: Failed to connect database." << db.lastError();
    } else {
        qDebug() << "Succeed to connect database.";
    }

    QSqlQuery query;

    if(!query.exec("create table if not exists users(username varchar primary key, password varchar, nickname varchar, groupslist varchar);")) {
        qDebug() << "Error: Fail to create table."<< query.lastError();
    } else {
        qDebug() << "Table users OK!";
    }

    if(!query.exec("create table if not exists groups(id int primary key, nickname varchar, owner varchar, members varchar);")) {
        qDebug() << "Error: Fail to create table."<< query.lastError();
    } else {
        qDebug() << "Table groups OK!";
    }
    query.finish();
}

Database::~Database() {
    db.close();
}

std::vector<User> Database::selectUser() {
    QSqlQuery query;
    std::vector<User> retval;
    if(query.exec("select * from users;")) {
        while(query.next()) {
            User user;
            user.username = query.value(0).toString();
            user.password = query.value(1).toString();
            user.nickname = query.value(2).toString();
            user.groupslist = query.value(3).toString();
            retval.push_back(user);
        }
        return retval;
    }
    qDebug() << query.lastError();
    query.finish();
    return retval;
}

User *Database::selectUserByUsername(QString username) {
    QSqlQuery query;
    query.prepare("select * from users where username == :username;");
    query.bindValue(":username", QString(username));
    query.exec();

    if(query.next()) {
        User *retval = new User;
        retval->username = username;
        retval->password = query.value(1).toString();
        retval->nickname = query.value(2).toString();
        retval->groupslist = query.value(3).toString();
        return retval;
    }

    qDebug() << query.lastError();
    query.finish();
    return NULL;
}

bool Database::insertUser(User user) {
    QSqlQuery query;
    query.prepare("insert into users (username, password, nickname, groupslist) values (:username, :password, :nickname, :groupslist);");
    query.bindValue(":username", user.username);
    query.bindValue(":password", user.password);
    query.bindValue(":nickname", user.nickname);
    query.bindValue(":groupslist", user.groupslist);

    if(query.exec()) {
        return true;
    }

    qDebug() << query.lastError();
    query.finish();
    return false;
}
