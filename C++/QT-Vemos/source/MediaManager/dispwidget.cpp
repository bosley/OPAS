#include "dispwidget.h"

dispwidget::dispwidget(QWidget *parent) : QLabel(parent)
{
    player = new VideoPlayer();
    QObject::connect(player, SIGNAL(processedImage(QImage)),
                     this, SLOT(updateScreen(QImage)));
}

dispwidget::~dispwidget()
{
    delete player;
}

void dispwidget::updateScreen(QImage img)
{
    if (!img.isNull())
    {
        this->setAlignment(Qt::AlignCenter);
        this->setPixmap(QPixmap::fromImage(img).scaled(this->size(),
                                                       Qt::KeepAspectRatio, Qt::FastTransformation));
    }
}

void dispwidget::startVideo(std::string video)
{
    if (!player->loadFile(video))
    {
        emit errorMessage("Unable to load video.");
        return;
    }
    else
    {
        player->Play();
        emit displayStarted();
    }
}

void dispwidget::startCamera(int camera)
{
    if (!player->loadCamera(camera))
    {
        emit errorMessage("Unable to load camera.");
        return;
    }
    else
    {
        player->Play();
        emit displayStarted();
    }
}

void dispwidget::stop()
{
    if(!player->isStopped())
    {
        player->Stop();
        emit displayStopped();
    }
}
