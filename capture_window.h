#ifndef CAPTURE_WINDOW_H_INCLUDED
#define CAPTURE_WINDOW_H_INCLUDED

#ifdef _WIN32
    #include <windows.h>
#endif
#include <opencv2/core/core.hpp>

void clrscr();

#ifdef _WIN32
std::pair <int, int> getResolution(HWND window);
HWND window_by_name(const char* class_name, const char* title_bar);
cv::Mat capture_window(HWND hwnd, int newWidth = 0, int newHeight = 0);
#else

#endif

#endif // CAPTURE_WINDOW_H_INCLUDED
