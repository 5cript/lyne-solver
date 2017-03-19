#include "lyne_graph_generator.h"
#include "lyne_solver.h"
#include "magic_mouse.h"
#include "solution_io.h"

#include "neural_helpers.h"

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <windows.h>
#include <exception>
#include <stdexcept>
#include <boost/filesystem.hpp>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace fs = boost::filesystem;

enum class Operation
{
    ExecuteSet = 1,
    GenerateSet = 2,
    SolveAllOnly = 3
};

bool loadSolution (fs::path where, std::vector <NodePath>& paths, std::pair <int, int>& resolution);
void dumpPaths (fs::path where, std::vector <NodePath> const& paths, std::pair <int, int> resolution,
                long long stepCounter, long long backtrackCounter);

int main( int argc, char** argv )
{
    auto hwnd = FindWindow("UnityWndClass", "LYNE");
    if (hwnd == 0)
    {
        std::cout << "Please start LYNE or unminimize it";
        return 1;
    }

    Operation op;
    std::string execGenOpt;
    std::cout << "Choose one operation\n";
    std::cout << "1: Execute Set\n";
    std::cout << "2: Generate Set\n";
    std::cout << "3: Solve All Only\n";

    std::getline (std::cin, execGenOpt);
    op = (Operation)std::stoi(execGenOpt);

    system("cls");

    bool wait = false;
    if (op != Operation::SolveAllOnly)
    {
        std::cout << "Wait for ok? (Don't solve it straight away and steal the mouse): ";
        std::string waitStr;
        std::getline(std::cin, waitStr);
        if (waitStr == "yes" || waitStr == "true" || waitStr == "1" || waitStr == "y" || waitStr == "Y")
            wait = true;
    }

    std::cout << "Please enter the set name: ";
    std::string set;
    std::getline(std::cin, set);
    fs::path setPath {"./" + set};
    fs::create_directory(setPath);

    int start = 100;
    std::cout << "Start at: ";
    std::cin >> start;
    std::cin.get();

    RECT size;
    GetWindowRect(hwnd, &size);

    WindowBorder border;
    std::pair <int, int> res = {
        size.right - size.left - 2 * border.barWidth,
        size.bottom - size.top - border.titleBarHeight - border.barWidth
    };

    std::vector <NodeMatrix> matrices;

    for (int counter = start; counter <= 25; ++counter)
    {
        if (op != Operation::SolveAllOnly)
        {
            for (int i = 0; i != 3; ++i)
            {
                std::cout << "Sleeping... " << i+1 << "\n";
                Sleep(1000);
            }
        }
        system("cls");

        switch (op)
        {
            case (Operation::ExecuteSet):
            {
                std::vector <NodePath> paths;
                std::pair <int, int> solRes;
                auto filePath = setPath / fs::path(std::to_string(counter) + ".lyne");
                if (!loadSolution(filePath, paths, solRes))
                {
                    std::cout << "No solution found.\n";
                    return 1;
                }
                if (solRes.first != res.first || solRes.second != res.second)
                {
                    std::cout << "Solution was recorded for different resolution.\n";
                    return 1;
                }

                if (wait)
                {
                    std::cout << "\nFound solution - Press button to continue";
                    std::cin.get();
                }
                DrawAllPaths(hwnd, paths, 70);
                break;
            }
            case (Operation::GenerateSet):
            {
                try
                {
                    LYNEGenerator gen;
                    auto LYNEMatrix = gen.generate();

                    gen.saveProcessed();

                    long long stepCounter = 0;
                    long long backtrackCounter = 0;

                    LYNESolver solver(LYNEMatrix, gen.getOriginal().clone());
                    auto paths = solver.solve(stepCounter, backtrackCounter);

                    auto filePath = setPath / fs::path(std::to_string(counter) + ".lyne");
                    dumpPaths (filePath, paths, res, stepCounter, backtrackCounter);

                    if (wait)
                    {
                        std::cout << "\nFound solution - Press button to continue";
                        std::cin.get();
                    }

                    DrawAllPaths(hwnd, paths, 70);
                }
                catch (std::exception const& exc)
                {
                    std::cout << exc.what();
                    return 1;
                }
                catch (...)
                {
                    std::cout << "An unknown error occured\n";
                    return 2;
                }
                break;
            }
            case (Operation::SolveAllOnly):
            {
                LYNEGenerator gen;
                auto NumberGrid = gen.generate();
                auto node = NumberGrid.get({Grid[counter - 1][1], Grid[counter - 1][0]});

                std::cout << counter - start + 1 << ": " << Grid[counter - 1][1] << " - " << Grid[counter - 1][0] << "\n";

                ClickOnce(hwnd, node.position);
                Sleep(1600);

                try
                {
                    LYNEGenerator gen;
                    auto LYNEMatrix = gen.generate();
                    matrices.push_back(LYNEMatrix);
                    ClickRelative(hwnd, {0.945, 0.026}); // measured
                    Sleep(1200);
                    ClickRelative(hwnd, {0.500, 0.666}); // measured
                    Sleep(1200);
                }
                catch (...)
                {
                    std::vector <std::vector <Node>> dum;
                    matrices.emplace_back(dum);
                }

                break;
            }
        }
    }

    if (op == Operation::SolveAllOnly)
    {
        for (int counter = start; counter <= 25; ++counter)
        {
            if (matrices[counter - start].getWidth() == 0)
                continue;

            long long stepCounter = 0;
            long long backtrackCounter = 0;

            LYNESolver solver(matrices[counter - start]);
            auto paths = solver.solve(stepCounter, backtrackCounter);

            auto filePath = setPath / fs::path(std::to_string(counter) + ".lyne");
            dumpPaths (filePath, paths, res, stepCounter, backtrackCounter);
        }
    }

    return 0;
}

void dumpPaths (fs::path where, std::vector <NodePath> const& paths, std::pair <int, int> resolution,
                long long stepCounter, long long backtrackCounter)
{
    auto solution = pathsToSolution(paths, resolution);
    solution.head.steps = stepCounter;
    solution.head.backtracks = backtrackCounter;
    saveSolutionToFile(where.string(), solution);
}

bool loadSolution (fs::path where, std::vector <NodePath>& paths, std::pair <int, int>& resolution)
{
    auto solution = loadSolutionFromFile(where.string());
    if (solution.paths.empty())
        return false;

    paths = solutionToPaths(solution);
    resolution = {solution.head.resolution.width, solution.head.resolution.height};
    return true;
}
