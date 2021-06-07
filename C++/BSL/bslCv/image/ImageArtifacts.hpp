//
//  Artifact - Something to draw on the screen, or something found on in the image
//             Common opencv shapes are implemented here to allow access to their shapes
//             through a common interface along-with special artifacts that can be added here.
//
#ifndef IMAGE_ARTIFACT_HPP
#define IMAGE_ARTIFACT_HPP

#include <map>
#include <vector>

#include <opencv2/opencv.hpp>

namespace IMAGE
{
    // --------------------------------------------------------------------
    //  Artifact
    // --------------------------------------------------------------------
    class ArtifactIf {
    public:
        // Setup artifact
        ArtifactIf(std::string name) : name(name) {}
        virtual ~ArtifactIf(){}

        // For reference purposes
        std::string getName() const { return name; }

        // Draw artifact on the matrix
        virtual void drawOnMatrix(cv::Mat &matrix) = 0;

    protected:
        std::string name;
    };

    namespace ARTIFACT {

        // --------------------------------------------------------------------
        //  Artifact - Rectangle
        // --------------------------------------------------------------------
        class Rectangle : public ArtifactIf {
        public:
            // Setup artifact
            Rectangle(std::string name, cv::Point pt1, cv::Point pt2, cv::Scalar color, int thickness) :
                ArtifactIf(name),
                point1(pt1), point2(pt2), color(color), thickness(thickness){
            }

            // Draw on the matrix
            virtual void drawOnMatrix(cv::Mat &matrix) override {
                cv::rectangle(matrix, point1, point2, color, thickness);
            }

        private:
            cv::Point  point1;
            cv::Point  point2;
            cv::Scalar color;
            int        thickness;
        };

        // --------------------------------------------------------------------
        //  Artifact - Ellipse
        // --------------------------------------------------------------------
        class Ellipse : public ArtifactIf {
        public:
            // Setup artifact
            Ellipse(std::string name, cv::Point center,  cv::Size axes,
                    double angle,     double startAngle, double endAngle, cv::Scalar color, int thickness) :
                ArtifactIf(name),
                center(center),     axes(axes),
                angle(angle),       startAngle(startAngle),
                endAngle(endAngle), color(color),
                thickness(thickness){
            }

            // Draw on the matrix
            virtual void drawOnMatrix(cv::Mat &matrix) override {
                cv::ellipse(matrix, center, axes, angle, startAngle, endAngle, color, thickness);
            }

        private:
            cv::Point  center;
            cv::Size   axes;
            double     angle;
            double     startAngle;
            double     endAngle;
            cv::Scalar color;
            int        thickness;
        };

        // --------------------------------------------------------------------
        //  Artifact - Line
        // --------------------------------------------------------------------
        class Line : public ArtifactIf {
        public:
            // Setup artifact
            Line(std::string name, cv::Point start, cv::Point end, cv::Scalar color, int thickness) :
                ArtifactIf(name), start(start), end(end), color(color), thickness(thickness){
            }

            // Draw on the matrix
            virtual void drawOnMatrix(cv::Mat &matrix) override {
                cv::line(matrix, start, end, color, thickness);
            }

        private:
            cv::Point  start;
            cv::Point  end;
            cv::Scalar color;
            int        thickness;
        };
    } // End artifacts
} // End Image

#endif // IMAGEARTIFACT_HPP
