#include "shape.h"

#include <cmath>
#include <opencv2/imgproc/imgproc.hpp>

cv::Size getShapeSize(Shape const& shape)
{
    return shape.boundingRect.size();
}
