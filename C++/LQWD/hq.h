#ifndef HQ_H
#define HQ_H

#include <QMainWindow>
#include <QtCore/QtGlobal>

namespace Ui
{
    class HQ;
}

class Bkons;

class HQ : public QMainWindow
{
    Q_OBJECT

public:
    explicit HQ(QWidget *parent = 0);
    ~HQ();

private:
    Ui::HQ *ui;

    Bkons *bkonsole;
};

#endif // HQ_H
