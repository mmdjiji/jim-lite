#ifndef LOADWINDOW_H
#define LOADWINDOW_H

#include <QMainWindow>

namespace Ui {
class LoadWindow;
}

class LoadWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoadWindow(QWidget *parent = 0);
    ~LoadWindow();

private:
    Ui::LoadWindow *ui;
};

#endif // LOADWINDOW_H
