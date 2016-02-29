#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QThread>
#include <QMainWindow>
#include "framework/engine/engine.h"

#define STATE_IDLE 0
#define STATE_SEND 1
#define STATE_WAIT 2
#define STATE_PROC 3

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void indicateJobRetrieved(QString type, QString data);

private slots:
    void on_acceptButton_clicked();

    void on_cancelButton_clicked();

private:
    Ui::MainWindow *ui;
    Engine mainEngine;

    int currentState;

    void setState(int state);
};

#endif // MAINWINDOW_H
