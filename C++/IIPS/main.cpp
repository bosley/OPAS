/* 
 * File:   main.cpp
 * Author: josh
 *
 * Created on November 14, 2018, 8:41 PM
 */

#include <cstdlib>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <map>
#include <memory>
#include <string>

#include "LOG/aixlog.hpp"
#include "UTIL/ImageSourceIF.h"
#include "UTIL/CameraSource.h"
#include "IPF/Series.h"
#include "IPF/DataBank.h"


#include "ALGO/Sobel.h"
#include "ALGO/ContourAnalyzation.h"

using namespace std;

#define KEY_q 113
#define KEY_b 98
#define KEY_r 114

void testContinuousContour(std::vector<cv::Point> poi)
{
    /*
        .       .       .       0       0       0       .       0       .
        .       0       .       .       0       .       .       0       .
        .       0       .       .       0       .       .       0       .
        .       .       0       .       0       .       .       .       .
        .       .       .       0       .       .       .       .       .
        .       .       .       .       .       .       .       .       .
        .       .       .       .       .       0       0       0       0
     */
    
    
    std::vector<cv::Point> test; 
    
    test.push_back(cv::Point(3,0));
    test.push_back(cv::Point(4,0));
    test.push_back(cv::Point(5,0));
    test.push_back(cv::Point(1,1));
    test.push_back(cv::Point(4,1));
    test.push_back(cv::Point(1,2));
    test.push_back(cv::Point(4,2));
    test.push_back(cv::Point(2,3));
    test.push_back(cv::Point(4,3));
    test.push_back(cv::Point(3,4));
    
    test.push_back(cv::Point(7,0));
    test.push_back(cv::Point(7,1));
    test.push_back(cv::Point(7,2));
    
    test.push_back(cv::Point(7,6));
    test.push_back(cv::Point(8,6));
    test.push_back(cv::Point(6,6));
    test.push_back(cv::Point(5,6));
    
    std::vector< std::vector<cv::Point> > continuous;
    ALGO::CONTOURS::Continuous(test, continuous, 2, 9, 7);
}

void overlayPoi(std::vector<cv::Point> poi, cv::Mat &image)
{
    for(auto i = poi.begin(); i != poi.end(); ++i)
    {
        image.at<uchar>((*i).x,(*i).y) = 255;
    }
    
    // ----------------- TESTING AREA
    
    std::vector< std::vector<cv::Point> > continuous;
    
    // rows = x because OpenCV ....
   // ALGO::CONTOURS::Continuous(poi, continuous, 10, image.rows, image.cols);
    
    // -----------------
}

/*
 * 
 */
int main(int argc, char** argv) 
{
    //
    //  Setup Logger
    //
    AixLog::Log::init<AixLog::SinkCout>(
        AixLog::Severity::trace, AixLog::Type::normal
    );
    
    //
    // Create a video source
    //
    UTIL::CameraSource source(0);
    source.open();
    
    // Image map
    std::shared_ptr< IPF::DataBank > db = std::make_shared< IPF::DataBank >();
    
    //
    //  Setup process series
    //
    IPF::Series series;
    
    ALGO::Sobel sobelProcess;
    sobelProcess.defineDb(db);
    sobelProcess.defineSeriesCallback(&series);
    
    series.addStep(&sobelProcess);
    
    
    //
    //  Process Loop
    //
    LOG(INFO) << "Starting: Use 'q' on Display Window to exit.";
    
    //
    //  Create Window
    //
    cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
    
    bool running = true;
    while(running)
    {
        //
        //  Show source image
        //
        cv::Mat frame = source.read();
        
        if(frame.empty())
        {
            LOG(WARNING) << "Empty frame from source" << std::endl;
        }
        else
        {
            //  Get gray frame, and store the frames in the imageMap
            cv::Mat gray;
            cv::Mat grayBlur;
            cv::cvtColor(frame, gray, CV_BGR2GRAY);
            cv::blur(gray, grayBlur, cv::Size(10,10) );
            
            db->imageMap.setItem(std::string("frame"), frame);
            db->imageMap.setItem(std::string("frameGray"), gray);
            db->imageMap.setItem(std::string("frameGrayBlur"), grayBlur);
            
            //
            //  Process Series
            //
            while( series.process()){};
            
            //
            //  Display 
            //
            std::vector<cv::Point> points;
            
            if(db->poiMap.getItem(std::string("sobelContours"), points))
            {
                overlayPoi(points, gray);
            }
         
            // Display
            cv::imshow("Display window", gray);
        }
        
        //
        //  Key input
        //
        switch(cv::waitKey(1))
        {
            case KEY_q:
                LOG(INFO) << "'q' pressed, closing" << std::endl;
                running = false;
                break;
            case KEY_b:
                LOG(INFO) << "'b' pressed" << std::endl;
                break;
            case KEY_r:
                LOG(INFO) << "'r' pressed" << std::endl;
                break;
        };
    }

    //
    //  Cleanup
    //
    source.close();
    cv::destroyAllWindows();
    return 0;
}

