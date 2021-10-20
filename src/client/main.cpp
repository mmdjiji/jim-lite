#include "loadwindow.h"
#include "signinwindow.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QElapsedTimer>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    LoadWindow loadWindow;
    loadWindow.show();

    int delayTime = 3;
    QElapsedTimer timer;
    timer.start();
    while(timer.elapsed() < (delayTime * 1000)) {
         app.processEvents();
    }
    loadWindow.close();

    QFile file(":/dark.qss");
    file.open(QFile::ReadOnly | QFile::Text);
    QTextStream stream(&file);
    app.setStyleSheet(stream.readAll());

    SignInWindow signInWindow;
    signInWindow.show();

    return app.exec();
}
