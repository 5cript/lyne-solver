#include "node_matrix.h"

NodeMatrix::NodeMatrix (std::vector <std::vector <Node> > nodes)
    : nodes_(nodes)
{
}

std::vector <NodeShape> NodeMatrix::getShapeList() const
{
    std::vector <NodeShape> shapes;
    for (auto const& i : nodes_)
    {
        for (auto const& j : i)
        {
            if (std::find(std::begin(shapes), std::end(shapes), j.shape) == std::end(shapes))
                if (j.shape != NodeShape::ValenceRestricted && j.shape != NodeShape::Nothing)
                    shapes.push_back(j.shape);
        }
    }
    return shapes;
}

boost::optional <std::pair <MatrixPosition, MatrixPosition> > NodeMatrix::getStartEndPair(NodeShape shape) const
{
    std::vector <MatrixPosition> epa;

    for (MatrixPosition::value_type i = 0; i != static_cast <MatrixPosition::value_type> (nodes_.size()); ++i)
    {
        for (MatrixPosition::value_type j = 0; j != static_cast <MatrixPosition::value_type> (nodes_[i].size()); ++j)
        {
            if (nodes_[i][j].shape == shape && nodes_[i][j].requiredValence == 1)
            {
                epa.push_back(MatrixPosition{i, j});

                if (epa.size() == 2)
                    return {std::make_pair (epa[0], epa[1])};
            }
        }
    }
    return boost::none;
}

std::vector <MatrixPosition> NodeMatrix::getAdjacent(MatrixPosition const& origin, std::vector <MatrixPosition> const& blackList) const
{
    // cannot be
    //if (!isNode(origin))
    //    throw std::runtime_error ("Origin is not a node, therefore this request is invalid");

    std::vector <MatrixPosition> pos;

    auto addIfExists = [&, this](MatrixPosition p) {
        if (isNode(p) && std::find(std::begin(blackList), std::end(blackList), p) == std::end(blackList))
            pos.push_back(p);
    };

    addIfExists({origin.x - 1, origin.y - 1});
    addIfExists({origin.x    , origin.y - 1});
    addIfExists({origin.x + 1, origin.y - 1});

    addIfExists({origin.x - 1, origin.y    });
    addIfExists({origin.x + 1, origin.y    });

    addIfExists({origin.x - 1, origin.y + 1});
    addIfExists({origin.x    , origin.y + 1});
    addIfExists({origin.x + 1, origin.y + 1});

    return pos;
}
