#ifndef NODE_H_INCLUDED
#define NODE_H_INCLUDED

#include <opencv2/core/core.hpp>
#include <vector>

enum class NodeShape
{
    Nothing = 0x0,
    Triangle = 0xA8DBA8,
    Diamond = 0x86863B,
    Square = 0x5C78C2,
    Pentagon = 0x1,
    Hexagon = 0x2,
    ValenceRestricted = 0xD8DBA7
};
cv::Vec4b ShapeToVector(NodeShape shape);
NodeShape ShapeFromVector(cv::Vec4b const& vect);

struct Node
{
    cv::Point position = {};
    NodeShape shape = NodeShape::Nothing;
    int requiredValence = 0;
    int valence = 0;
    std::vector <std::pair <Node*, NodeShape> > connections;
};

#endif // NODE_H_INCLUDED
