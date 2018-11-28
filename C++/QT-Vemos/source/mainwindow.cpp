#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_reloadCameras_clicked()
{
    sm.reloadCameras();
    ui->cameraCombo->clear();
    for(int i = 0; i < sm.getAvailableCameras(); i++)
    {
        ui->cameraCombo->addItem(QString::number(i));
    }
}

void MainWindow::on_loadVideoFile_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                              tr("Open Video"), ".",
                                              tr("Video Files (*.avi *.mpg *.mp4)"));
    sm.setSelectedVideo(filename.toStdString().data());
    ui->currVideo->setText("..."+filename.mid(filename.length()-30));
}

void MainWindow::on_launchCamera_clicked()
{
    sm.setSelectedCamera(ui->cameraCombo->currentText().toInt());

    screen *s = new screen(&sm, this);
    connect(s, SIGNAL(completed()), this, SLOT(processComplete()));
    s->setAttribute(Qt::WA_DeleteOnClose, true);
    s->show();
    s->launchCamera();

    toggleUI(false);
}

void MainWindow::on_launchVideoFile_clicked()
{
    screen *s = new screen(&sm, this);
    connect(s, SIGNAL(completed()), this, SLOT(processComplete()));
    s->setAttribute(Qt::WA_DeleteOnClose, true);
    s->show();
    s->launchVideo();

    toggleUI(false);
}

void MainWindow::processComplete()
{
    toggleUI(true);
}

void MainWindow::errorMessage(QString m)
{

}

void MainWindow::toggleUI(bool state)
{
    ui->loadVideoFile->setEnabled(state);
    ui->launchCamera->setEnabled(state);
    ui->reloadCameras->setEnabled(state);
    ui->launchVideoFile->setEnabled(state);
    ui->cameraCombo->setEnabled(state);
}
