#ifndef NODE_MATRIX_H_INCLUDED
#define NODE_MATRIX_H_INCLUDED

#include "node.h"

#include <boost/optional.hpp>

struct MatrixPosition
{
    using value_type = std::make_signed <std::size_t>::type;

    value_type x;
    value_type y;

    friend bool operator== (MatrixPosition const& lhs, MatrixPosition const& rhs);
    friend bool operator!= (MatrixPosition const& lhs, MatrixPosition const& rhs);
};

inline bool operator== (MatrixPosition const& lhs, MatrixPosition const& rhs)
{
    return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!= (MatrixPosition const& lhs, MatrixPosition const& rhs)
{
    return lhs.x != rhs.x || lhs.y != rhs.y;
}

class NodeMatrix
{
public:
    NodeMatrix (std::vector <std::vector <Node> > nodes);
    std::vector <NodeShape> getShapeList() const;
    boost::optional <std::pair <MatrixPosition, MatrixPosition> > getStartEndPair(NodeShape shape) const;
    std::vector <MatrixPosition> getAdjacent(MatrixPosition const& origin, std::vector <MatrixPosition> const& blackList) const;

    inline bool isNode(MatrixPosition position) const
    {
        if (position.x >= getWidth() || position.x < 0)
            return false;
        if (position.y >= getHeight() || position.y < 0)
            return false;

        return get(position).shape != NodeShape::Nothing;
    }

    inline int getWidth() const
    {
        return static_cast <int> (nodes_.size());
    }

    inline int getHeight() const
    {
        return static_cast <int> (nodes_[0].size());
    }

    inline Node& get(MatrixPosition position)
    {
        return nodes_[position.x][position.y];
    }

    inline Node get(MatrixPosition position) const
    {
        return nodes_[position.x][position.y];
    }

    inline std::vector <std::vector <Node> >& getNodes()
    {
        return nodes_;
    }

    inline std::vector <std::vector <Node> > const& getNodes() const
    {
        return nodes_;
    }

private:
    std::vector <std::vector <Node> /* one line */> nodes_;
};

#endif // NODE_MATRIX_H_INCLUDED
