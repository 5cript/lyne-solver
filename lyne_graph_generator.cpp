#include "lyne_graph_generator.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>
#include <utility>
#include <functional>

const int xGridVariance = 15;
const int yGridVariance = 20;

const double colorSpotRatioX = 0.17;
const double colorSpotRatioY = 0.17;

const double numberDotRatio = 0.16;

LYNEGenerator::LYNEGenerator()
{
    auto hwnd = FindWindow("UnityWndClass", "LYNE");
    original_ = capture_window(hwnd);
    cv::imwrite("./captured.png", original_);

    if( !original_.data )
        throw std::runtime_error("Could not find LYNE window or render it");

    crop(original_);
}

void LYNEGenerator::showProcessed()
{
    cv::imshow("Processed Image", processed_);
}

void LYNEGenerator::showOriginal()
{
    cv::imshow("Original Image", original_);
}

void LYNEGenerator::saveProcessed()
{
    cv::imwrite("./processed.png", processed_);
}

void LYNEGenerator::createGrid(std::vector <Node>& nodes, std::vector <int>& xGrid, std::vector <int>& yGrid)
{
    // sort by Y
    std::sort(std::begin(nodes), std::end(nodes), [](Node const& lhs, Node const& rhs) {
        return lhs.position.y < rhs.position.y;
    });

    std::vector <std::pair <std::vector <std::reference_wrapper <Node>>, int> > YList;

    std::vector <std::reference_wrapper <Node>> YAccum;
    int YAverage = nodes.front().position.y;
    for (auto i = std::begin(nodes); i != std::end(nodes); ++i)
    {
        if (std::abs (i->position.y - YAverage) <= yGridVariance)
        {
            YAccum.push_back(std::ref(*i));
            YAverage = (YAverage + i->position.y) / 2;
        }
        else
        {
            YList.push_back(std::make_pair (YAccum, YAverage));
            YAccum.clear();
            YAccum.push_back(std::ref(*i));
            YAverage = i->position.y;
        }
    }
    // finally:
    YList.push_back(std::make_pair (YAccum, YAverage));

    for (auto& i : YList)
    {
        yGrid.push_back(i.second);
        // sort by X
        std::sort(std::begin(i.first), std::end(i.first), [](Node const& lhs, Node const& rhs) {
            return lhs.position.x < rhs.position.x;
        });
        for (auto& j : i.first)
        {
            bool foundSimilar = false;
            for (auto const& x : xGrid) {
                if (std::abs(x - j.get().position.x) < xGridVariance) {
                    foundSimilar = true;
                    break;
                }
            }
            if (!foundSimilar)
                xGrid.push_back(j.get().position.x);

            j.get().position.y = i.second;
        }
    }

    // normalize X
    for (auto& node : nodes)
    {
        for (auto const& xg : xGrid)
        {
            if (std::abs(node.position.x - xg) < xGridVariance)
            {
                node.position.x = xg;
                break;
            }
        }
    }

    std::sort(std::begin(xGrid), std::end(xGrid));
    std::sort(std::begin(yGrid), std::end(yGrid));
}

void LYNEGenerator::classifyShapesIntoNodes(std::vector <Shape> const& shapes, std::vector <Node>& nodes)
{
    for (auto const& i : shapes)
    {
        Node node;

        auto center = i.center;

        // this is correct, all ratios are depending on the height.
        cv::Point offset = {center.x + static_cast <decltype(center.x)>(static_cast <double> (i.boundingRect.size().height) * colorSpotRatioX),
                            center.y + static_cast <decltype(center.y)>(static_cast <double> (i.boundingRect.size().height) * colorSpotRatioY)};

        auto centerColor = original_.at<cv::Vec4b>(center);

        decltype(centerColor) nodeColor;

        if (centerColor == cv::Vec4b{0xDF, 0xF1, 0xE9, 0xFF})
        {
            // END-START
            circle(processed_, center, 3, {0x0, 0xFF, 0x0}, 5);

            nodeColor = original_.at<cv::Vec4b>(offset);
            node.requiredValence = 1;
        }
        else
        {
            // INTERMEDIATE / 1_VERT / 2_VERT / 3_VERT / 4_VERT
            circle(processed_, center, 3, {0x0, 0x0, 0xFF}, 5);

            nodeColor = centerColor;
            node.requiredValence = 2;
        }

        circle(processed_, offset, 3, cv::Scalar(nodeColor), 5);

        node.shape = ShapeFromVector(nodeColor);

        if (node.shape == NodeShape::ValenceRestricted)
        {
            cv::Point top =    {center.x,
                                center.y - static_cast <decltype(center.y)>(static_cast <double> (i.boundingRect.size().height) * numberDotRatio)};

            cv::Point bottom = {center.x,
                                center.y + static_cast <decltype(center.y)>(static_cast <double> (i.boundingRect.size().height) * numberDotRatio)};

            cv::Point left =   {center.x - static_cast <decltype(center.x)>(static_cast <double> (i.boundingRect.size().height) * numberDotRatio),
                                center.y};

            cv::Point right =  {center.x + static_cast <decltype(center.x)>(static_cast <double> (i.boundingRect.size().height) * numberDotRatio),
                                center.y};

            cv::Vec4b valenceDotColor = cv::Vec4b {0x9A, 0xBD, 0x79, 0xFF};

            int valence = 0;
            valence += (valenceDotColor == original_.at<cv::Vec4b>(left)) ? 1 : 0;
            valence += (valenceDotColor == original_.at<cv::Vec4b>(right)) ? 1 : 0;
            valence += (valenceDotColor == original_.at<cv::Vec4b>(top)) ? 1 : 0;
            valence += (valenceDotColor == original_.at<cv::Vec4b>(bottom)) ? 1 : 0;

            circle(processed_, left, 2, cv::Scalar(cv::Vec4b{0xFF, 0x0, 0x0, 0xFF}), 3);
            circle(processed_, right, 2, cv::Scalar(cv::Vec4b{0xFF, 0x0, 0x0, 0xFF}), 3);
            circle(processed_, top, 2, cv::Scalar(cv::Vec4b{0xFF, 0x0, 0x0, 0xFF}), 3);
            circle(processed_, bottom, 2, cv::Scalar(cv::Vec4b{0xFF, 0x0, 0x0, 0xFF}), 3);

            node.requiredValence = valence * 2;
        }

        node.position = center;
        nodes.push_back(node);
    }
}

std::vector <std::vector <Node> > LYNEGenerator::createMatrix(std::vector <Node> nodes, std::vector <int> const& xGrid, std::vector <int> const& yGrid)
{
    std::vector <std::vector <Node> > result;
    result.resize(xGrid.size());
    for (auto& i : result)
    {
        i.resize(yGrid.size());
    }

    for (auto const& i : nodes)
    {
        auto xGridPos = std::distance(std::begin(xGrid), std::find(std::begin(xGrid), std::end(xGrid), i.position.x));
        auto yGridPos = std::distance(std::begin(yGrid), std::find(std::begin(yGrid), std::end(yGrid), i.position.y));

        result[xGridPos][yGridPos] = i;
    }

    return result;
}

NodeMatrix LYNEGenerator::generate()
{
    std::vector <Shape> shapes;
    preprocess(original_, processed_);
    detectShapes(processed_, processed_, shapes);
    cv::cvtColor( processed_, processed_, CV_GRAY2BGR );

    std::vector <Node> nodes;
    std::vector <int> xGrid;
    std::vector <int> yGrid;

    classifyShapesIntoNodes(shapes, nodes);
    createGrid(nodes, xGrid, yGrid);

    return {createMatrix(nodes, xGrid, yGrid)};

    // 0xDFF1E9 = center
}

cv::Mat LYNEGenerator::getOriginal() const
{
    return original_;
}
