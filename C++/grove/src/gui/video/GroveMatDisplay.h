#ifndef GROVEMATDISPLAY_H
#define GROVEMATDISPLAY_H

#include <QMainWindow>
#include <QCloseEvent>
#include <thread>
#include "modules/bus/Bus.h"
#include "gui/video/CVMatDisplayDriver.h"

namespace Ui {
class GroveMatDisplay;
}

class GroveMatDisplay : public QMainWindow
{
    Q_OBJECT

public:
    explicit GroveMatDisplay(GUI::CVMatDisplayDriver *driver, QWidget *parent = nullptr);
    ~GroveMatDisplay();

private:
    Ui::GroveMatDisplay *ui;

    GUI::CVMatDisplayDriver *driver;
    std::thread dispThread;

    void closeEvent(QCloseEvent * event);

    void displayLoop();
};

#endif // GROVEMATDISPLAY_H
