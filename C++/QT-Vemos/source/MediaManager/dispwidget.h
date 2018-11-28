#ifndef DISPWIDGET_H
#define DISPWIDGET_H

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include "videoplayer.h"

class dispwidget : public QLabel
{
    Q_OBJECT
public:
    explicit dispwidget(QWidget *parent = nullptr);
    ~dispwidget();
    void startVideo(std::string video);
    void startCamera(int camera);
    void stop();

signals:    
    void displayStopped();
    void displayStarted();
    void errorMessage(QString error);

public slots:
    void updateScreen(QImage image);

private:
    VideoPlayer *player;
};

#endif // DISPWIDGET_H
