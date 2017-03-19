#include "matrix_cursor.h"

MatrixCursor::MatrixCursor (MatrixPosition start, MatrixPosition target, NodeShape shape)
    : position(start)
    , start(start)
    , target(target)
    , shape(shape)
    , previous()
    , stepBlackList()
{}

MatrixCursor::MatrixCursor (MatrixPosition currentPosition, MatrixPosition start, MatrixPosition target, NodeShape shape)
    : position(currentPosition)
    , start(start)
    , target(target)
    , shape(shape)
    , previous()
    , stepBlackList()
{

}

MatrixCursor::MatrixCursor (
    MatrixPosition position,
    MatrixPosition start,
    MatrixPosition target,
    NodeShape shape,
    std::shared_ptr <MatrixCursor> previous,
    std::vector <MatrixPosition> stepBlackList
)
    : position(position)
    , start(start)
    , target(target)
    , shape(shape)
    , previous(previous)
    , stepBlackList(stepBlackList)
{
}

MatrixCursor decendCursor(MatrixCursor const& cursor, MatrixPosition newPosition)
{
    return {
        newPosition,
        cursor.start,
        cursor.target,
        cursor.shape,
        std::shared_ptr <MatrixCursor> (new MatrixCursor{cursor}),
        {
            cursor.position // dont walk backwards, you silly
        }
    };
}

bool backtrackCursor(MatrixCursor const& cursor, MatrixCursor& backtracked)
{
    if (!cursor.previous.get())
        return false;

    MatrixCursor& c = *cursor.previous.get();
    c.stepBlackList.push_back(cursor.position);
    backtracked = c;
    return true;
}

bool hasReachedTarget(MatrixCursor const& cursor)
{
    return cursor.target == cursor.position;
}
