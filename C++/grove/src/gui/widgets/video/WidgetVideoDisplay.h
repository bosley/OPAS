#ifndef DISPWIDGET_H
#define DISPWIDGET_H

#include <QWidget>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QSize>

#include <opencv2/videoio.hpp>

#define VIDEO_DISPLAY_WIDGET_RESIZABLE 0

class VideoDisplay : public QLabel
{
    Q_OBJECT
public:
    explicit VideoDisplay(QWidget *parent = nullptr);
    ~VideoDisplay();

    void imageReady(cv::Mat frame);
};

#endif // DISPWIDGET_H
