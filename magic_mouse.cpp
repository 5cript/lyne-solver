#include "magic_mouse.h"

#include <vector>

void DrawAllPaths(HWND window, std::vector <NodePath> const& paths, int delay, WindowBorder border)
{
    RECT rect;
    GetWindowRect(window, &rect);
    BringWindowToTop(window);
    SetFocus(window);

    //SetCursorPos(rect.left + paths[0][0].x + border.barWidth, rect.top + paths[0][0].y + border.titleBarHeight);

    std::vector <INPUT> inputs;

    int xOffset = border.barWidth + rect.left;
    int yOffset = rect.top + border.titleBarHeight;

    auto makeMouseMoveInput = [&](int x, int y)
    {
        INPUT input {0};
        input.type = INPUT_MOUSE;
        input.mi.mouseData = 0;
        input.mi.dx =  x*(65536./GetSystemMetrics(SM_CXSCREEN));//x being coord in pixels
        input.mi.dy =  y*(65536./GetSystemMetrics(SM_CYSCREEN));//y being coord in pixels
        input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN;
        return input;
    };

    auto makeMouseButtonRelease = [&]()
    {
        INPUT input {0};
        input.type = INPUT_MOUSE;
        input.mi.mouseData = 0;
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        input.mi.mouseData=0;
        input.mi.dwExtraInfo=0;
        input.mi.time=0;
        return input;
    };

    for (auto const& path : paths)
    {
        for (auto const& step : path)
        {
            inputs.push_back(makeMouseMoveInput(step.x + xOffset, step.y + yOffset));
        }
        inputs.push_back(makeMouseButtonRelease());
    }

    for (auto& i : inputs)
    {
        SendInput(1, &i, sizeof(INPUT));
        Sleep(delay);
    }
}

void ClickOnce (HWND window, cv::Point pos, int clickDelay)
{
    std::vector <cv::Point> p = {pos};
    DrawAllPaths(window, {p}, clickDelay);
}

void ClickRelative (HWND window, std::pair <double, double> relative, int clickDelay)
{
    RECT rect;
    GetWindowRect(window, &rect);
    WindowBorder wb;

    auto w = rect.right - rect.left - wb.barWidth*2;
    auto h = rect.bottom - rect.top - wb.titleBarHeight - wb.barWidth;
    ClickOnce (window, {(int)(relative.first * (double)w), (int)(relative.second * (double)h)}, clickDelay);
}
