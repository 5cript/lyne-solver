#ifndef LYNE_GRAPH_GENERATOR_H_INCLUDED
#define LYNE_GRAPH_GENERATOR_H_INCLUDED

#include "capture_window.h"
#include "recognition.h"
#include "node.h"
#include "node_matrix.h"

class LYNEGenerator
{
public:
    LYNEGenerator();
    void showProcessed();
    void showOriginal();
    void saveProcessed();
    NodeMatrix generate();
    void solve();
    cv::Mat getOriginal() const; // const is a lie

private:
    void createGrid(std::vector <Node>& nodes, std::vector <int>& xGrid, std::vector <int>& yGrid);
    void classifyShapesIntoNodes(std::vector <Shape> const& shapes, std::vector <Node>& nodes);
    std::vector <std::vector <Node> > createMatrix(std::vector <Node> nodes, std::vector <int> const& xGrid, std::vector <int> const& yGrid);

private:
    cv::Mat original_;
    cv::Mat processed_;
};

#endif // LYNE_GRAPH_GENERATOR_H_INCLUDED
