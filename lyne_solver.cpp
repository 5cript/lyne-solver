#include "lyne_solver.h"
#include "matrix_cursor.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <random>
#include <algorithm>

LYNESolver::LYNESolver (NodeMatrix matrix, cv::Mat const& solutionDisplay)
    : LYNEMatrix_(matrix)
    , orig_()
    , solutionDisplay_()
{
    solutionDisplay.copyTo(orig_);
}

bool LYNESolver::canConnect(NodeMatrix& matrix, MatrixPosition n1, MatrixPosition n2, NodeShape type)
{
    // OOR checks
    if (n1.x >= matrix.getWidth() || n2.x >= matrix.getWidth())
        return false;
    if (n1.y >= matrix.getHeight() || n2.y >= matrix.getHeight())
        return false;

    // Check for correct color
    Node& node1 = matrix.get(n1);
    Node& node2 = matrix.get(n2);

    // Check for max valence
    if (node1.requiredValence == node1.valence || node2.requiredValence == node2.valence)
        return false;

    // Manhattan Distance checks (algo does not do this)
    // if (!(std::abs(n1.x - n2.x) <= 1 && std::abs(n1.y - n2.y) <= 1))
    //    return false;

    auto isConnected = [&](MatrixPosition a, MatrixPosition b)
    {
        if (!matrix.isNode(a))
            return false;

        if (!matrix.isNode(b))
            return false;

        Node& node1 = matrix.get(a);
        Node& node2 = matrix.get(b);

        for (auto const& i : node1.connections)
            if (i.first == &node2)
                return true;

        return false;
    };

    // Existing connection check
    if (isConnected(n1, n2))
        return false;

    // Check for diagonal block
    if (std::abs(n1.x - n2.x) == 1 && std::abs(n1.y - n2.y) == 1) // is diagonal move
    {
        if (isConnected ({n2.x, n1.y}, {n1.x, n2.y}))
            return false;
    }

    if ((node1.shape != type && node1.shape != NodeShape::ValenceRestricted) ||
        (node2.shape != type && node2.shape != NodeShape::ValenceRestricted))
    {
        return false;
    }

    return true;
}

void LYNESolver::forceConnect(NodeMatrix& matrix, MatrixPosition n1, MatrixPosition n2, NodeShape type)
{
    Node& node1 = matrix.get(n1);
    Node& node2 = matrix.get(n2);

    node1.connections.push_back(std::make_pair (&node2, type));
    node2.connections.push_back(std::make_pair (&node1, type));

    node1.valence++;
    node2.valence++;
}

bool LYNESolver::tryConnect(NodeMatrix& matrix, MatrixPosition n1, MatrixPosition n2, NodeShape type)
{
    if (canConnect(matrix, n1, n2, type))
    {
        forceConnect(matrix, n1, n2, type);
        return true;
    }
    return false;
}

void LYNESolver::forceDisconnect(NodeMatrix& matrix, MatrixPosition n1, MatrixPosition n2)
{
    Node& node1 = matrix.get(n1);
    Node& node2 = matrix.get(n2);

    auto disconnect = [&](Node& node, Node* otherNode) {
        for (auto iter = std::begin(node.connections); iter != std::end(node.connections); ++iter)
        {
            if (iter->first == otherNode)
            {
                node.connections.erase(iter);
                break;
            }
        }
        node.valence--;
    };

    disconnect(node1, &node2);
    disconnect(node2, &node1);
}

bool LYNESolver::isSolution(NodeMatrix const& mat)
{
    bool isSol = true;
    for (auto const& i : (mat.getNodes()))
    {
        for (auto const& j : i)
        {
            if (j.valence != j.requiredValence)
            {
                isSol = false;
                goto STEP_OUT;
            }
        }
    }

    STEP_OUT:
    {
        return isSol;
    }
}

bool LYNESolver::couldBeShapeSolution(NodeMatrix const& mat, NodeShape shape)
{
    bool isSol = true;
    for (auto const& i : (mat.getNodes()))
    {
        for (auto const& j : i)
        {
            if (j.shape != shape)
                continue;

            if (j.valence != j.requiredValence)
            {
                isSol = false;
                goto STEP_OUT;
            }
        }
    }

    STEP_OUT:
    {
        return isSol;
    }
}

std::vector <NodePath> LYNESolver::solve(long long& stepCounter, long long& backtrackCounter)
{
    // make a copy of the game board used for modification:
    auto matrix = LYNEMatrix_;

    // get all shape types in the board:
    auto shapes = matrix.getShapeList();

    // make the cursors for each shape
    std::vector <MatrixCursor> cursors;

    for (auto const& i : shapes)
    {
        boost::optional <std::pair <MatrixPosition, MatrixPosition> > endpoints = matrix.getStartEndPair(i);
        if (!endpoints)
            throw std::runtime_error("board is invalid");

        cursors.push_back ({
            endpoints.get().first,
            endpoints.get().second,
            i
        });
    }

    // now start backtracking algorithm
    auto makeStep = [&](MatrixCursor const& cursor, MatrixCursor& nextCursor) -> bool
    {
        auto adjacentPositions = matrix.getAdjacent(cursor.position, cursor.stepBlackList);

        //std::random_shuffle(std::begin(adjacentPositions), std::end(adjacentPositions));

        MatrixPosition next;
        auto findPossibleStep = [&]() -> bool
        {
            for (auto const& i : adjacentPositions)
            {
                // blacklisted are already filtered
                if (tryConnect(matrix, cursor.position, i, cursor.shape))
                {
                    next = i;
                    return true;
                }
            }
            return false;
        };

        if (findPossibleStep())
        {
            stepCounter++;
            nextCursor = decendCursor(cursor, next);
            return true;
        }
        else
            return false;
    };

    auto drawSolution = [&]() {
        solutionDisplay_ = orig_.clone();

        for (auto const& i : cursors)
        {
            MatrixCursor const* c = &i;
            while (c->previous)
            {
                line(solutionDisplay_, matrix.get(c->position).position, matrix.get(c->previous.get()->position).position, cv::Scalar(ShapeToVector(i.shape)), 10);

                c = c->previous.get();
            }
        }
    };

    auto backtrack = [&](MatrixCursor& cursor) -> bool {
        backtrackCounter++;

        auto pcpy = cursor.position;
        auto backwards = backtrackCursor(cursor, cursor);
        if (backwards)
            forceDisconnect(matrix, pcpy, cursor.position);
        else
            return false;
        return true;
    };

    // solve puzzle:
    int activeCursor = 0;
    int solC = 0;
    while (!isSolution(matrix))
    {
        if (stepCounter % 10000 == 0)
        {
            std::cout << "Steps: " << stepCounter << " - Backtracks: " << backtrackCounter << "\n";
        }

        bool reached = true;
        auto& nC = cursors[activeCursor];
        for (;;)
        {
            bool r = false;
            if (!makeStep(nC, nC) && !(r = hasReachedTarget(nC)) && !couldBeShapeSolution(matrix, nC.shape))
                if (!backtrack(nC))
                {
                    reached = false;
                    break;
                }
            if (r)
                break;
        }

        if (reached)
            activeCursor++;
        else
        {
            nC.stepBlackList.clear();
            nC.stepBlackList.reserve(16);
            activeCursor--;
            if (activeCursor == -1)
                throw std::runtime_error("No solution");
            auto& tC = cursors[activeCursor];
            if (!backtrack(tC))
                throw std::runtime_error("No solution");
        }

        if (activeCursor == -1)
            throw std::runtime_error("No solution");

        if (activeCursor == static_cast <int> (cursors.size()) && !isSolution(matrix))
        {
            activeCursor--; // reverse change
            if (!backtrack(nC))
            {
                throw std::runtime_error("No solution");
            }
        }
    }

    //drawSolution();
    //imshow("Solution", solutionDisplay_);

    std::vector <NodePath> pathes;
    for (auto const& i : cursors)
    {
        NodePath path;
        MatrixCursor const* c = &i;
        path.push_back(matrix.get(c->position).position);
        while (c->previous)
        {
            c = c->previous.get();
            path.push_back(matrix.get(c->position).position);
        }
        pathes.push_back(path);
    }


    std::cout << "\n----------------------FINAL-------------------------\n";
    std::cout << "Steps: " << stepCounter << " - Backtracks: " << backtrackCounter << "\n";
    std::cout << "----------------------------------------------------\n";

    return pathes;
}
