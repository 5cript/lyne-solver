#include "recognition.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <algorithm>
#include <iostream>
#include <type_traits>

/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void cannyThreshold(cv::Mat const& src, cv::Mat& dst, CannyOptions const& options)
{
    using namespace cv;

    Mat detected_edges;

    // Create a matrix of the same type and size as src (for dst)
    dst.create( src.size(), src.type() );

    // Reduce noise with a kernel 3x3
    blur( src, detected_edges, Size(3,3) );

    // Canny detector
    Canny( detected_edges, detected_edges, options.lowThreshold, options.lowThreshold*options.ratio, options.kernel_size );

    // Using Canny's output as a mask, we display our result
    dst = Scalar::all(0);

    src.copyTo( dst, detected_edges);
}


/**
 * Helper function to find a cosine of angle between vectors
 * from pt0->pt1 and pt0->pt2
 */
static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

/**
 * Helper function to display text in the center of a contour
 */
void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point> const& contour)
{
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	cv::Rect r = cv::boundingRect(contour);

	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255,255,255), CV_FILLED);
	cv::putText(im, label, pt, fontface, scale, CV_RGB(0,0,0), thickness, 8);
}

void crop(cv::Mat& img)
{
    cv::Size s = img.size();
    cv::Rect crop {10, 10, s.width - 20, s.height - 20};

    img = img(crop);
}

void preprocess(cv::Mat const& img, cv::Mat& result)
{
    cv::Mat intermediate;
    img.copyTo(intermediate);

    for (int x = 0; x != intermediate.size().width; ++x)
    for (int y = 0; y != intermediate.size().height; ++y)
    {
        auto col = intermediate.at <cv::Vec4b>(cv::Point{x, y});
        if (col == cv::Vec4b{0xA4, 0xC4, 0x89, 0xFF})
        {
            intermediate.at <cv::Vec4b> (cv::Point{x, y}) = cv::Vec4b {0x94, 0xBD, 0x79, 0xFF};
        }
    }

    auto translate = [](cv::Mat const& img, int offsetX, int offsetY) {
        cv::Mat res;
        cv::Mat trans_mat = (cv::Mat_<double>(2,3) << 1, 0, offsetX, 0, 1, offsetY);
        cv::warpAffine(img, res, trans_mat, img.size());
        return res;
    };

    auto overlayTranslated = [&translate](cv::Mat& img) {
        cv::Mat transX = translate(img, 1, 0);
        cv::Mat transY = translate(img, 0, 1);

        addWeighted(img, 1., transX, 1., 0., img);
        addWeighted(img, 1., transY, 1., 0., img);
    };

    cv::Mat grayscaled;

    // Convert the image to grayscale
    cvtColor(intermediate, grayscaled, CV_BGR2GRAY );

    cannyThreshold(grayscaled, result);

    overlayTranslated(result);
}

void detectShapes(cv::Mat const& src, cv::Mat& dst, std::vector <Shape>& shapes, bool printLabels)
{
	//cv::Mat src = cv::imread("polygon.png");
	if (src.empty())
		return;

    std::vector<std::vector<cv::Point> > contours;

	// Find contours
	cv::findContours(src.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	std::vector<cv::Point> approx;
	dst = src.clone();

	std::vector <std::size_t> remList;

	auto addShape = [&](std::string const& ID, std::decay<decltype(contours[0])>::type const& contour) {
        if (printLabels)
            setLabel(dst, ID, contour);

        cv::Rect r = cv::boundingRect(contour);
        cv::Point center(r.x + ((r.width) / 2), r.y + ((r.height) / 2));

        if (r.height < src.size().height * 0.04)
            return; // too small

        // correct center down for triangles
        if (ID == "TRI")
            center.y += r.height * 0.15f;

        shapes.push_back ({
            ID,
            contour,
            r,
            center
        });
	};

	for (std::size_t i = 0; i < contours.size(); i++)
	{
		// Approximate contour with accuracy proportional
		// to the contour perimeter
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

		// Skip small or non-convex objects
		if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
			continue;

		if (approx.size() == 3)
		    addShape("TRI", contours[i]);

		else if (approx.size() >= 4 && approx.size() <= 6)
		{
			// Number of vertices of polygonal curve
			int vtc = approx.size();

			// Get the cosines of all corners
			std::vector<double> cos;
			for (int j = 2; j < vtc+1; j++)
				cos.push_back(angle(approx[j%vtc], approx[j-2], approx[j-1]));

			// Sort ascending the cosine values
			std::sort(cos.begin(), cos.end());

			// Get the lowest and the highest cosine
			double mincos = cos.front();
			double maxcos = cos.back();

			// Use the degrees obtained above and the number of vertices
			// to determine the shape of the contour
			if (vtc == 4 && mincos >= -0.1 && maxcos <= 0.3)
                addShape("RECT", contours[i]);
			else if (vtc == 5 && mincos >= -0.34 && maxcos <= -0.27)
                addShape("PENTA", contours[i]);
			else if (vtc == 6 && mincos >= -0.55 && maxcos <= -0.45)
                addShape("HEXA", contours[i]);
		}
		else
		{
			// Detect and label circles
			double area = cv::contourArea(contours[i]);
			cv::Rect r = cv::boundingRect(contours[i]);
			int radius = r.width / 2;

			if (std::abs(1 - ((double)r.width / r.height)) <= 0.2 &&
			    std::abs(1 - (area / (CV_PI * std::pow(radius, 2)))) <= 0.2)
            {
                addShape("CIR", contours[i]);
            }
		}
	}

    std::vector<std::vector<cv::Point> > filtered;
	for (std::size_t i = 0; i != contours.size(); ++i) {
        if (std::find(std::begin(remList), std::end(remList), i) != std::end(remList))
            filtered.push_back(contours[i]);
	}

	//cv::imshow("dst", dst);
	cv::waitKey(0);
}
