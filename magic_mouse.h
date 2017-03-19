#ifndef MAGIC_MOUSE_H_INCLUDED
#define MAGIC_MOUSE_H_INCLUDED

#include "path.h"

#if _WIN32
#   include <windows.h>
#endif

#if _WIN32
struct WindowBorder
{
    int titleBarHeight = 26;
    int barWidth = 3;
};

void DrawAllPaths (HWND window, std::vector <NodePath> const& paths, int delay = 100, WindowBorder border = {});
void ClickOnce (HWND window, cv::Point pos, int clickDelay = 50);
void ClickRelative (HWND window, std::pair <double, double> relative, int clickDelay = 50);

#endif
constexpr static int Grid[][2] = {
    {0, 0},
    {1, 0}, {1, 1}, {1, 2}, {1, 3},
    {2, 0}, {2, 1}, {2, 2}, {2, 3},
    {3, 0}, {3, 1}, {3, 2}, {3, 3},
    {4, 0}, {4, 1}, {4, 2}, {4, 3},
    {5, 0}, {5, 1}, {5, 2}, {5, 3},
    {6, 0}, {6, 1}, {6, 2}, {6, 3}
};

#endif // MAGIC_MOUSE_H_INCLUDED
