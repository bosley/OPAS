#include "WidgetVideoDisplay.h"

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <QDebug>
#include <iostream>

VideoDisplay::VideoDisplay(QWidget *parent) : QLabel(parent)
{
    this->setAlignment(Qt::AlignCenter);
}

VideoDisplay::~VideoDisplay()
{
}

void VideoDisplay::imageReady(cv::Mat frame)
{
    if(frame.empty())
    {
        return;
    }

    QImage img;
    if (frame.channels()== 3)
    {
        cv::Mat RGBFrame;
        cv::cvtColor(frame, RGBFrame, CV_BGR2RGB);
        img = QImage(reinterpret_cast<const unsigned char*>(RGBFrame.data),
                          RGBFrame.cols,RGBFrame.rows,QImage::Format_RGB888);
    }
    else
    {
        img = QImage(reinterpret_cast<const unsigned char*>(frame.data),
                             frame.cols,frame.rows,QImage::Format_Indexed8);
    }

    if (!img.isNull())
    {
#if VIDEO_DISPLAY_WIDGET_RESIZABLE > 0
        this->setPixmap(QPixmap::fromImage(img).scaled(this->size(),
                                                       Qt::KeepAspectRatio, Qt::FastTransformation));
#else
        this->setPixmap(QPixmap::fromImage(img));
#endif
    }
    else
    {
        std::cout << "Image is null =( \n";
    }
}

