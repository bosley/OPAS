#include "messagebox.h"
#include "ui_messagebox.h"

MessageBox::MessageBox(QString message, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageBox)
{
    ui->setupUi(this);
    ui->textBrowser->insertPlainText(message);
}

MessageBox::~MessageBox()
{
    delete ui;
}

void MessageBox::on_pushButton_clicked()
{
    this->close();
}
