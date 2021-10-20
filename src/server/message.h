#ifndef MESSAGE_H
#define MESSAGE_H

#include <QString>
#include <QDateTime>

class Message {
public:
    Message();
    QDateTime time; // send time
    QString src; // recv from
    QString dst; // send to
    QString type;
    QString text;
};

#endif // MESSAGE_H
