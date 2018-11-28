#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

// Class derived from online tutorial
// Unable to find at the time of uploading,
// feel free to add credit and submit merge.

#include <string>
#include <QMutex>
#include <QImage>
#include <QThread>
#include <QWaitCondition>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;

class VideoPlayer : public QThread
{
    Q_OBJECT
public:
    VideoPlayer(QObject *parent = 0);
    ~VideoPlayer();
    bool loadFile(std::string file);
    bool loadCamera(int camera);
    void Play();
    void Stop();
    bool isStopped() const;

signals:
      void processedImage(const QImage &image);

protected:
     void run();
     void msleep(int ms);

private:
    bool stop;
    QMutex mutex;
    QWaitCondition condition;
    Mat frame;
    int frameRate;
    VideoCapture capture;
    Mat RGBframe;
    QImage img;
};

#endif // VIDEOPLAYER_H
