#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDateTime>
#include <QMainWindow>
#include "engine/engine.h"

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
    void updateUser(QString);
    void jobComplete();

private slots:
    void on_pushButton_clicked();

signals:
    void sendEngineSignal(char);

private:
    Ui::MainWindow *ui;

    bool state;
    Engine engine;
    void flipInterface();
};

#endif // MAINWINDOW_H
