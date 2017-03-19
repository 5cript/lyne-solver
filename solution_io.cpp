#include "solution_io.h"

#include <fstream>

Point::Point (int x, int y)
    : x(x)
    , y(y)
{
}
Resolution::Resolution (int width, int height)
    : width(width)
    , height(height)
{
}

Solution pathsToSolution(std::vector <NodePath> const& paths, std::pair <int, int> resolution)
{
    Solution sol;
    for (auto const& i : paths)
    {
        std::vector <Point> path;
        for (auto const& j : i)
            path.push_back(Point{j.x, j.y});

        sol.paths.push_back(path);
    }
    sol.head.resolution = Resolution{resolution.first, resolution.second};
    return sol;
}
std::vector <NodePath> solutionToPaths(Solution const& solution)
{
    std::vector <NodePath> paths;
    for (auto const& i : solution.paths)
    {
        NodePath path;
        for (auto const& j : i)
            path.push_back(cv::Point{j.x, j.y});

        paths.push_back(path);
    }
    return paths;
}
void saveSolutionToFile(std::string const& file, Solution const& solution)
{
    std::ofstream f {file, std::ios_base::binary};
    f << '{';
    JSON::stringify(f, "solution", solution, JSON::ProduceNamedOutput) << '}';
}
Solution loadSolutionFromFile(std::string const& file)
{
    Solution sol;

    std::ifstream in {file, std::ios_base::binary};
    if (!in.good())
        return {};

    auto tree = JSON::parse_json(in);
    JSON::parse(sol, "solution", tree);

    return sol;
}
