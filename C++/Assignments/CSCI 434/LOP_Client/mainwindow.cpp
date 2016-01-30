#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connect the engine jobRet signal to the local slot for information display
    connect(&mainEngine, SIGNAL(jobRet(QString, QString)), this, SLOT(indicateJobRetrieved(QString, QString)));

    setState(STATE_IDLE);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::indicateJobRetrieved(QString type, QString data)
{
    // Called by MainEngine to let us know that a new job
    // has been retrieved and is being worked on
    setState(STATE_PROC);
    QString outp = "Job type handed from server : [" + type + "]";
    outp += "\n\n" + data;
    ui->informationOut->setPlainText(outp);
}

void MainWindow::on_acceptButton_clicked()
{
    setState(STATE_SEND);
}

void MainWindow::on_cancelButton_clicked()
{
    setState(STATE_IDLE);
}

void MainWindow::setState(int state)
{
    currentState = state;
    ui->cancelButton->setEnabled(true);

    switch(state)
    {
    case STATE_IDLE:
    {
        ui->cancelButton->setEnabled(false);
        this->ui->acceptButton->setEnabled(true);
        this->ui->labelClientState->setText("IDLE");
        this->ui->informationOut->setPlainText("Press Accept to send request for new job from the server");
        break;
    }
    case STATE_WAIT:
    {
        this->ui->acceptButton->setEnabled(false);
        this->ui->labelClientState->setText("WAITING");
        break;
    }
    case STATE_SEND:
    {
        this->ui->acceptButton->setEnabled(false);
        this->ui->labelClientState->setText("REQUESTING");
        this->ui->informationOut->setPlainText("Sending the server a request");
        mainEngine.retrieveNewJob();

        // Wait a single second for display purposes
        QThread::sleep(2);
        break;
    }
    case STATE_PROC:
    {
        this->ui->acceptButton->setEnabled(false);
        this->ui->labelClientState->setText("PROC REQUEST");
        this->ui->informationOut->setPlainText("Server has sent a new job. That job is being worked on.");
        break;
    }
    /*
        Will need to add a few more state cases to allow for updatinf (sending solution back),
        in addition to writing functionality to inform the user what it is that is being worked on,
        and to show the result.

        Create functionality to request stats from server
    */
    default:
        break;
    }
}
