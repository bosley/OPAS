#include "hq.h"
#include "ui_hq.h"
#include "widgets/console/bkons.h"

HQ::HQ(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HQ)
{
    ui->setupUi(this);

    bkonsole = new Bkons();
    bkonsole->setEnabled(true);
    setCentralWidget(bkonsole);
    bkonsole->beginSession();
}

HQ::~HQ()
{
    delete ui;
}
