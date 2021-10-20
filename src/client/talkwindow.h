#ifndef TALKWINDOW_H
#define TALKWINDOW_H

#include <QMainWindow>
#include <QCloseEvent>
#include <QMimeData>
#include <QImageReader>
#include <QFileDialog>
#include <QMessageBox>

#include "protocol.h"
#include "record.h"

namespace Ui {
class TalkWindow;
}

class TalkWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit TalkWindow(QWidget *parent = 0);
    ~TalkWindow();
    void setProtocol(Protocol *protocol);
    void setOthers(bool isGroup, QString partner, QString partnerNickname);

private slots:
    void on_exitButton_clicked();
    void on_sendButton_clicked();
    void needRefreshMessage();

    void on_sendPicture_clicked();

    void on_sendFile_clicked();

protected:
    bool eventFilter(QObject *target, QEvent *event);
//    void dragEnterEvent(QDragEnterEvent *e);
//    void dropEvent(QDropEvent *e);

private:
    Ui::TalkWindow *ui;
    Protocol *protocol = nullptr;
    Record record;

    QString partnerUsername, partnerNickname;
    bool controlPressed = false;

    bool isGroup = false;
    void refreshMessage();
    void closeEvent(QCloseEvent *event);
};

#endif // TALKWINDOW_H
