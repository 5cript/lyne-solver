#ifndef RECOGNITION_H_INCLUDED
#define RECOGNITION_H_INCLUDED

#include <opencv2/core/core.hpp>

#include "shape.h"

struct CannyOptions
{
    int edgeThresh = 2;
    int lowThreshold = 30;
    int const max_lowThreshold = 120;
    int ratio = 2;
    int kernel_size = 3;
};

void crop(cv::Mat& img);
void preprocess(cv::Mat const& img, cv::Mat& result);
void cannyThreshold(cv::Mat const& src, cv::Mat& dst, CannyOptions const& options = {});
void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point> const& contour);
void detectShapes(cv::Mat const& src, cv::Mat& dst, std::vector<Shape>& shapes, bool printLabels = false);

#endif // RECOGNITION_H_INCLUDED
