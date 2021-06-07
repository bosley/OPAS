#include <iostream>
#include <string>

#include <stdio.h>
#include <opencv2/opencv.hpp>

int main(int argc, char** argv)
{
    cv::VideoCapture cap;
    
    if(!cap.open(0))
    {
        return 0;
    }

    for(;;)
    {
          cv::Mat frame;
          cap >> frame;
          if( frame.empty() ) break; 
          imshow("Raw open cv usage", frame);
          if( cv::waitKey(10) == 27 ) break;
    }

    return 0;
}