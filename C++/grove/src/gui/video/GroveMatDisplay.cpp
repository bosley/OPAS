#include "GroveMatDisplay.h"
#include "ui_GroveMatDisplay.h"

GroveMatDisplay::GroveMatDisplay(GUI::CVMatDisplayDriver *driver, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GroveMatDisplay)
{
    ui->setupUi(this);
    this->driver = driver;
    this->driver->setDisplayWidget(ui->widget);
    dispThread = std::thread(&GroveMatDisplay::displayLoop, this);
}

GroveMatDisplay::~GroveMatDisplay()
{
    delete ui;
}

void GroveMatDisplay::closeEvent(QCloseEvent * event)
{
    this->driver->shutdown();
    this->dispThread.join();
    event->accept();
}

void GroveMatDisplay::displayLoop()
{
    this->driver->start();
}
