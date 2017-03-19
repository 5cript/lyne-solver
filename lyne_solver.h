#ifndef LYNE_SOLVER_H_INCLUDED
#define LYNE_SOLVER_H_INCLUDED

#include "node_matrix.h"
#include "path.h"

#include <type_traits>

class LYNESolver
{
public:
    LYNESolver (NodeMatrix matrix, cv::Mat const& solutionDisplay = {});
    std::vector <NodePath> solve(long long& stepCounter, long long& backtrackCounter);

private:
    bool isSolution(NodeMatrix const& mat);
    bool couldBeShapeSolution(NodeMatrix const& mat, NodeShape shape);
    bool canConnect(NodeMatrix& matrix, MatrixPosition n1, MatrixPosition n2, NodeShape type);
    bool tryConnect(NodeMatrix& matrix, MatrixPosition n1, MatrixPosition n2, NodeShape type);
    void forceConnect(NodeMatrix& matrix, MatrixPosition n1, MatrixPosition n2, NodeShape type);
    void forceDisconnect(NodeMatrix& matrix, MatrixPosition n1, MatrixPosition n2);

private:
    NodeMatrix LYNEMatrix_;
    cv::Mat orig_;
    cv::Mat solutionDisplay_;
};

#endif // LYNE_SOLVER_H_INCLUDED
