#ifndef SHAPE_H_INCLUDED
#define SHAPE_H_INCLUDED

#include <opencv2/core/core.hpp>
#include <string>

struct Shape
{
    std::string type;
    std::vector <cv::Point> contour;
    cv::Rect boundingRect;
    cv::Point center;
};

cv::Size getShapeSize(Shape const& shape);
Shape cleanContour(Shape const& shape, int dist = 5);


#endif // SHAPE_H_INCLUDED
