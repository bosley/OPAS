#include "videoplayer.h"

VideoPlayer::VideoPlayer(QObject *parent)
    : QThread(parent)
{
    stop = true;
}

VideoPlayer::~VideoPlayer()
{
    mutex.lock();
    stop = true;
    capture.release();
    condition.wakeOne();
    mutex.unlock();
    wait();
}

bool VideoPlayer::loadCamera(int camera)
{
    capture.open(camera);

    if (capture.isOpened())
    {
        frameRate = (int) capture.get(CV_CAP_PROP_FPS);
        return true;
    }
    else
    {
        return false;
    }
}

bool VideoPlayer::loadFile(std::string file)
{
    capture.open(file);
    if (capture.isOpened())
    {
        frameRate = (int) capture.get(CV_CAP_PROP_FPS);
        return true;
    }
    else
    {
        return false;
    }
}

void VideoPlayer::Play()
{
    if (!isRunning())
    {
        if (isStopped()){
            stop = false;
        }
        start(LowPriority);
    }
}

void VideoPlayer::Stop()
{
    stop = true;
}

bool VideoPlayer::isStopped() const
{
    return stop;
}

void VideoPlayer::run()
{
    int delay = (1000/frameRate);
    while(!stop)
    {
        if (!capture.read(frame))
        {
            stop = true;
        }
        if (frame.channels()== 3){
            cv::cvtColor(frame, RGBframe, CV_BGR2RGB);
            img = QImage((const unsigned char*)(RGBframe.data),
                              RGBframe.cols,RGBframe.rows,QImage::Format_RGB888);
        }
        else
        {
            img = QImage((const unsigned char*)(frame.data),
                                 frame.cols,frame.rows,QImage::Format_Indexed8);
        }
        emit processedImage(img);
        this->msleep(delay);
    }
}

void VideoPlayer::msleep(int ms)
{
    struct timespec ts = { ms / 1000, (ms % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
}
