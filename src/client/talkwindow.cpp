#include "talkwindow.h"
#include "ui_talkwindow.h"

TalkWindow::TalkWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::TalkWindow) {
    ui->setupUi(this);
    ui->sendArea->installEventFilter(this);
//    ui->sendArea->setAcceptDrops(false);
//    this->setAcceptDrops(true);
}

TalkWindow::~TalkWindow() {
    delete ui;
}

void TalkWindow::setProtocol(Protocol *protocol) {
    this->protocol = protocol;
    connect(this->protocol, SIGNAL(needRefreshMessage()), this, SLOT(needRefreshMessage()));
}

void TalkWindow::refreshMessage() {
    // friend message
    ui->msgArea->setText("");
    std::vector<Message> message = record.selectRecordByUsername(protocol->me.username, this->partnerUsername);
    for(std::vector<Message>::iterator it = message.begin(); it != message.end(); ++it) {
        if(it->type == "plain") {
            QString format = "[%1] %2\n%3";
            ui->msgArea->append(format.arg(it->time.toString("yyyy-MM-dd hh:mm:ss")).arg((it->src == protocol->me.username)?protocol->me.nickname:protocol->friendsNickname[it->src]).arg(it->text));
        } else if(it->type == "picture") {
            QString format = "[%1] %2\n";
            ui->msgArea->append(format.arg(it->time.toString("yyyy-MM-dd hh:mm:ss")).arg((it->src == protocol->me.username)?protocol->me.nickname:protocol->friendsNickname[it->src]));

            QPixmap imagePixmap;
            imagePixmap.loadFromData(QByteArray::fromBase64(it->text.toUtf8()));
            QImage image = imagePixmap.toImage();
            QTextDocument *textDocument = ui->msgArea->document();
            textDocument->addResource(QTextDocument::ImageResource, QUrl("[图片]"), QVariant(image));
            QTextCursor cursor = ui->msgArea->textCursor();
            QTextImageFormat imageFormat;
            int nSmall = image.width() / 150;
            imageFormat.setWidth(image.width()/nSmall);
            imageFormat.setHeight(image.height()/nSmall);
            imageFormat.setName("[图片]");
            cursor.insertImage(imageFormat);
        } else if(it->type == "file") {
            QString fileMessage = it->text;
            QString fileName = fileMessage.split(",")[0];
            QByteArray fileValue = QByteArray::fromBase64(fileMessage.split(",")[1].toUtf8());
            QFile file(fileName);
            file.open(QFile::WriteOnly);
            QDataStream in(&file);
            in.writeRawData(fileValue, fileValue.size());
            file.close();
            QString format = "[%1] %2\n[文件]已保存至%3";
            ui->msgArea->append(format.arg(it->time.toString("yyyy-MM-dd hh:mm:ss")).arg((it->src == protocol->me.username)?protocol->me.nickname:protocol->friendsNickname[it->src]).arg(fileName));
        }
    }
    QTextCursor cursor = ui->msgArea->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->msgArea->setTextCursor(cursor);
}

void TalkWindow::needRefreshMessage() {
    this->refreshMessage();
}

void TalkWindow::setOthers(bool isGroup, QString partnerUsername, QString partnerNickname) {
    this->isGroup = isGroup;
    this->partnerUsername = partnerUsername;
    this->partnerNickname = partnerNickname;
    this->setWindowTitle("与" + this->partnerNickname + "的对话窗口");
    if(!this->isGroup) {
        this->setFixedSize(780, this->height()); // friend is 780
    } else {
        this->setFixedSize(980, this->height()); // group is 980
    }
    refreshMessage();
}

void TalkWindow::on_exitButton_clicked() {
    this->close();
}

void TalkWindow::closeEvent(QCloseEvent *event) {
    ui->msgArea->setText("");
    ui->sendArea->setText("");
    protocol->openWindowWith = "";
    event->accept();
}

void TalkWindow::on_sendButton_clicked() {
    // friend message
    if(ui->sendArea->toPlainText() != "") {
        Message message;
        message.time = QDateTime::currentDateTime();
        message.src = protocol->me.username;
        message.dst = this->partnerUsername;
        message.type = "plain";
        message.text = ui->sendArea->toPlainText();
        protocol->sendFriendMessage(message);
        record.insertRecordByUsername(protocol->me.username, this->partnerUsername, message);

        ui->sendArea->setText("");
        refreshMessage();
    } else {
        QMessageBox::critical(NULL, "JIM-Lite", "请输入要发送的消息！");
    }
}

bool TalkWindow::eventFilter(QObject *target, QEvent *event) {
    if(target == ui->sendArea) {
        if(event->type() == QEvent::KeyPress) {
            QKeyEvent *key = static_cast<QKeyEvent *>(event);
            if(key->key() == Qt::Key_Return || key->key() == Qt::Key_Enter) {
                if(this->controlPressed) {
                    ui->sendArea->append("");
                } else {
                    on_sendButton_clicked();
                }
                return true;
            } else if(key->key() == Qt::Key_Control) {
                this->controlPressed = true;
            }
        } else if(event->type() == QEvent::KeyRelease) {
            QKeyEvent *key = static_cast<QKeyEvent *>(event);
            if(key->key() == Qt::Key_Control) {
                this->controlPressed = false;
            }
        }
    }
    return QWidget::eventFilter(target,event);
}

//void TalkWindow::dragEnterEvent(QDragEnterEvent *e)
//{
//    if(e->mimeData()->hasFormat("text/uri-list")) {
//        e->acceptProposedAction();
//    } else {
//        e->ignore();
//    }
//}

//void TalkWindow::dropEvent(QDropEvent *e)
//{
//    QList<QUrl> urls = e->mimeData()->urls();
//    if(urls.empty())
//        return;
//    for (int i=0; i<urls.size(); i++) {
//        QString fileName = urls[i].toLocalFile();
//        QString extendName = fileName.right(4).toLower();
//        QUrl Uri(QString("file://%1").arg(fileName));
//        if(extendName.compare(".jpg")==0 || extendName.compare(".jpeg")==0 || extendName.compare(".png")==0 || extendName.compare(".bmp")==0 || extendName.compare(".gif")==0) {
//            QImage image = QImageReader(fileName).read();
//            QTextDocument * textDocument = ui->sendArea->document();
//            textDocument->addResource(QTextDocument::ImageResource, Uri, QVariant(image));
//            QTextCursor cursor = ui->sendArea->textCursor();
//            QTextImageFormat imageFormat;
//            int nSmall = image.width() / 150;
//            imageFormat.setWidth( image.width()/nSmall );
//            imageFormat.setHeight( image.height()/nSmall );
//            imageFormat.setName( Uri.toString() );
//            cursor.insertImage(imageFormat);
//            // qDebug() << fileName;
//        } else {
//            QImage image = QImageReader (":/picture/assets/file.png").read();
//            QTextDocument * textDocument = ui->sendArea->document();
//            textDocument->addResource(QTextDocument::ImageResource, Uri, QVariant(image));
//            QTextCursor cursor = ui->sendArea->textCursor();
//            QTextImageFormat imageFormat;
//            imageFormat.setWidth(image.width());
//            imageFormat.setHeight(image.height());
//            imageFormat.setName(Uri.toString());
//            cursor.insertImage(imageFormat);
//            // qDebug() << fileName;
//        }
//    }
//}

void TalkWindow::on_sendPicture_clicked() {
    QString arg = "";
    QString picturePath = QFileDialog::getOpenFileName(this, "Open File", "", "Images(*.png *.jpg *.jpeg *.bmp *.gif);", &arg);
    if(picturePath != "") {
        QFile file(picturePath);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Truncate)) {
            return;
        }
        QByteArray fileValue = file.readAll();
        file.close();
        QString fileBase64 = QString(fileValue.toBase64());

        Message message;
        message.time = QDateTime::currentDateTime();
        message.src = protocol->me.username;
        message.dst = this->partnerUsername;
        message.type = "picture";
        message.text = fileBase64;
        protocol->sendFriendMessage(message);
        record.insertRecordByUsername(protocol->me.username, this->partnerUsername, message);
        refreshMessage();
    }
}

void TalkWindow::on_sendFile_clicked() {
    QString arg = "";
    QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", "All Files(*);", &arg);
    if(filePath != "") {
        QFile file(filePath);
        if(!file.open(QIODevice::ReadOnly | QIODevice::Truncate)) {
            return;
        }
        QByteArray fileValue = file.readAll();
        file.close();
        QString fileBase64 = QString(fileValue.toBase64());
        QString fileName = QFileInfo(filePath).fileName();

        Message message;
        message.time = QDateTime::currentDateTime();
        message.src = protocol->me.username;
        message.dst = this->partnerUsername;
        message.type = "file";
        message.text = fileName + "," + fileBase64;
        protocol->sendFriendMessage(message);
        record.insertRecordByUsername(protocol->me.username, this->partnerUsername, message);
        refreshMessage();
    }
}
