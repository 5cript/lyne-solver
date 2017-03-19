#ifndef SOLUTION_IO_H_INCLUDED
#define SOLUTION_IO_H_INCLUDED

#ifndef Q_MOC_RUN // A Qt workaround, for those of you who use Qt
#   include "SimpleJSON/parse/jsd.hpp"
#   include "SimpleJSON/parse/jsd_convenience.hpp"
#   include "SimpleJSON/stringify/jss.hpp"
#   include "SimpleJSON/stringify/jss_fusion_adapted_struct.hpp"
#endif

#include <string>
#include <vector>

#include <opencv2/core/core.hpp>

#include "path.h"

struct Point : public JSON::Stringifiable <Point>
             , public JSON::Parsable <Point>
{
    int x;
    int y;

    Point (int x = 0, int y = 0);
};

BOOST_FUSION_ADAPT_STRUCT
(
    Point,
    (int, x)
    (int, y)
)

struct Resolution : public JSON::Stringifiable <Resolution>
                  , public JSON::Parsable <Resolution>
{
    int width;
    int height;

    Resolution (int width = 0, int height = 0);
};

BOOST_FUSION_ADAPT_STRUCT
(
    Resolution,
    (int, width)
    (int, height)
)

struct Head : public JSON::Stringifiable <Head>
            , public JSON::Parsable <Head>
{
    Resolution resolution;
    long long backtracks;
    long long steps;
};

BOOST_FUSION_ADAPT_STRUCT
(
    Head,
    (Resolution, resolution)
    (long long, backtracks)
    (long long, steps)
)

struct Solution : public JSON::Stringifiable <Solution>
                , public JSON::Parsable <Solution>
{
    Head head;
    std::vector <std::vector <Point> > paths;
};

BOOST_FUSION_ADAPT_STRUCT
(
    Solution,
    (Head, head)
    (std::vector <std::vector <Point> >, paths)
)

Solution pathsToSolution(std::vector <NodePath> const& paths, std::pair <int, int> resolution);
std::vector <NodePath> solutionToPaths(Solution const& solution);
void saveSolutionToFile(std::string const& file, Solution const& solution);
Solution loadSolutionFromFile(std::string const& file);

#endif // SOLUTION_IO_H_INCLUDED
