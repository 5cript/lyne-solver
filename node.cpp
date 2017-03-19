#include "node.h"

cv::Vec4b ShapeToVector(NodeShape shape)
{
    unsigned int value = static_cast <unsigned int> (shape);
    return {
        static_cast <unsigned char> ((value & 0xFF0000U) >> 16U),
        static_cast <unsigned char> ((value & 0x00FF00U) >>  8U),
        static_cast <unsigned char> ((value & 0x0000FFU) >>  0U),
        0xFF
    };
}

NodeShape ShapeFromVector(cv::Vec4b const& vect)
{
    if (ShapeToVector(NodeShape::Triangle) == vect)
        return NodeShape::Triangle;
    else if (ShapeToVector(NodeShape::Diamond) == vect)
        return NodeShape::Diamond;
    else if (ShapeToVector(NodeShape::Square) == vect)
        return NodeShape::Square;
    else if (ShapeToVector(NodeShape::Pentagon) == vect)
        return NodeShape::Pentagon;
    else if (ShapeToVector(NodeShape::Hexagon) == vect)
        return NodeShape::Hexagon;
    else if (ShapeToVector(NodeShape::ValenceRestricted) == vect)
        return NodeShape::ValenceRestricted;
    else
        return NodeShape::Nothing;
}
