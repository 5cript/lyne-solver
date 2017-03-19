#ifndef MATRIX_CURSOR_H_INCLUDED
#define MATRIX_CURSOR_H_INCLUDED

#include "node_matrix.h"
#include "node.h"

#include <boost/optional.hpp>
#include <memory>

struct MatrixCursor
{
    MatrixPosition position;
    MatrixPosition start;
    MatrixPosition target;
    NodeShape shape;
    std::shared_ptr <MatrixCursor> previous; // should be unique_ptr, buts this is just simpler.
    std::vector <MatrixPosition> stepBlackList;

    MatrixCursor (MatrixPosition start, MatrixPosition target, NodeShape shape);
    MatrixCursor (MatrixPosition currentPosition, MatrixPosition start, MatrixPosition target, NodeShape shape);
    MatrixCursor (
        MatrixPosition position,
        MatrixPosition start,
        MatrixPosition target,
        NodeShape shape,
        std::shared_ptr <MatrixCursor> previous,
        std::vector <MatrixPosition> stepBlackList
    );
};

MatrixCursor decendCursor(MatrixCursor const& cursor, MatrixPosition newPosition);
bool backtrackCursor(MatrixCursor const& cursor, MatrixCursor& backtracked);
bool hasReachedTarget(MatrixCursor const& cursor);

#endif // MATRIX_CURSOR_H_INCLUDED
