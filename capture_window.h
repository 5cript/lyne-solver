#ifndef CAPTURE_WINDOW_H_INCLUDED
#define CAPTURE_WINDOW_H_INCLUDED

#include <windows.h>
#include <opencv2/core/core.hpp>

cv::Mat capture_window(HWND hwnd, int newWidth = 0, int newHeight = 0);

#endif // CAPTURE_WINDOW_H_INCLUDED
