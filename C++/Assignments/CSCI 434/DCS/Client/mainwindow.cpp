#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // We aren't running yet, so init to false and create UI
    state = false;
    ui->setupUi(this);
    // Make connection with engine so it can post to the textBox
    connect(&engine, SIGNAL(userSignal(QString)), this, SLOT(updateUser(QString)));
    // Make connection with engine so that we can send signals to the engine :D
    connect(this, SIGNAL(sendEngineSignal(char)), &engine, SLOT(listenForSignal(char)));
    // Make connection with engine so it can inform us when its completed a job
    connect(&engine, SIGNAL(jobCompleted()), this, SLOT(jobComplete()));
}

MainWindow::~MainWindow()
{
    // Destructor, meh
    delete ui;
}

void MainWindow::updateUser(QString item)
{
    // Update user with some string given, in addition to a time stamp
    QDateTime today = QDateTime::currentDateTime();
    if(item != "STPBRK")
    {
        QString now = today.toString("yyyy-MM-dd hh:mm:ss");
        ui->plainTextEdit->insertPlainText("\n[" + now  + "]\t");
        ui->plainTextEdit->insertPlainText(item);
    }
    else
        ui->plainTextEdit->insertPlainText("\n\n");

    QTextCursor tc = ui->plainTextEdit->textCursor();
    tc.movePosition(QTextCursor::End);
    ui->plainTextEdit->setTextCursor(tc);
}
void MainWindow::on_pushButton_clicked()
{
    flipInterface();
}

void MainWindow::jobComplete()
{
    flipInterface();
}

void MainWindow::flipInterface()
{
    if(state)
    {
        // Set trun state to false, change button text, and send a stop signal to the engine
        state = false;
        ui->pushButton->setText("Initiate");
        emit sendEngineSignal('s');
        updateUser("STPBRK");
    }
    else
    {
        // Set run state to true, change button text, and tell engine to start retrieving jobs
        // Once engine.retieveNewJob() is initially called, engine will continue working until stop signal send
        state = true;
        ui->pushButton->setText("Stop");
        engine.retrieveNewJob();
    }
}
