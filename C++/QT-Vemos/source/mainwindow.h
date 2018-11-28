#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include "MediaManager/sourcemanager.h"
#include "screen.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_reloadCameras_clicked();
    void on_loadVideoFile_clicked();
    void on_launchCamera_clicked();
    void on_launchVideoFile_clicked();

public slots:
    void processComplete();
    void errorMessage(QString m);

private:
    Ui::MainWindow *ui;

    SourceManager sm;

    void toggleUI(bool state);
};

#endif // MAINWINDOW_H
