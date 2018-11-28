#ifndef SCREEN_H
#define SCREEN_H

#include <QMainWindow>
#include "screen.h"
#include <QMessageBox>
#include <QCloseEvent>
#include "MediaManager/sourcemanager.h"

namespace Ui {
class screen;
}

class screen : public QMainWindow
{
    Q_OBJECT

public:
    explicit screen(SourceManager *sm, QWidget *parent = 0);
    ~screen();
    void launchVideo();
    void launchCamera();

public slots:
    void errorMessage(QString m);

private slots:
    void on_actionStop_triggered();

signals:
    void completed();

private:
    Ui::screen *ui;
    SourceManager *lsm;

    void closeEvent(QCloseEvent * event);

};

#endif // SCREEN_H
