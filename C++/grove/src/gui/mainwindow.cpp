#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "gui/video/GroveMatDisplay.h"
#include "gui/video/CVMatDisplayDriver.h"

MainWindow::MainWindow(std::vector< GUI::CVMatDisplayDriver *> *videoDrivers, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->videoDrivers = videoDrivers;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::displayWindowClosed()
{

}

void MainWindow::on_actiondebug_triggered()
{
    // Open a new display for each driver sent in
    for(auto i =  this->videoDrivers->begin();
             i != this->videoDrivers->end();
           ++i)
    {
        GroveMatDisplay *s = new GroveMatDisplay((*i), this);
        s->setAttribute(Qt::WA_DeleteOnClose, true);
        s->show();
    }
}
