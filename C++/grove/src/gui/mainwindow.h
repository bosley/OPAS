#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <vector>

#include "gui/video/CVMatDisplayDriver.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(std::vector< GUI::CVMatDisplayDriver *> *videoDrivers, QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void displayWindowClosed();

private slots:
    void on_actiondebug_triggered();


private:
    Ui::MainWindow *ui;
    std::vector< GUI::CVMatDisplayDriver *> *videoDrivers;
};

#endif // MAINWINDOW_H
