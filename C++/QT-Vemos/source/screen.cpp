#include "screen.h"
#include "ui_screen.h"

screen::screen(SourceManager *sm, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::screen)
{
    lsm = sm;
    ui->setupUi(this);
    connect(ui->screenWidget, SIGNAL(errorMessage(QString)), this, SLOT(errorMessage(QString)));
}

screen::~screen()
{
    delete ui;
}

void screen::errorMessage(QString m)
{
    QMessageBox msgBox;
    msgBox.setText(m);
    msgBox.exec();
}

void screen::on_actionStop_triggered()
{
    ui->screenWidget->stop();
}

void screen::closeEvent(QCloseEvent *event)
{
    ui->screenWidget->stop();
    emit completed();
    event->accept();
}

void screen::launchVideo()
{
   ui->screenWidget->startVideo(lsm->getSelectedVideo());
}

void screen::launchCamera()
{
    ui->screenWidget->startCamera(lsm->getSelectedCamera());
}
