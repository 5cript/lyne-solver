#include "capture_window.h"
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

cv::Mat capture_window(HWND hwnd, int newWidth, int newHeight)
{
    HDC hwindowDC, hwindowCompatibleDC;

    int height, width, srcheight, srcwidth;
    HBITMAP hbwindow;
    cv::Mat src;
    BITMAPINFOHEADER  bi;

    hwindowDC = GetDC (hwnd);
    hwindowCompatibleDC = CreateCompatibleDC (hwindowDC);
    SetStretchBltMode (hwindowCompatibleDC, COLORONCOLOR);

    RECT windowsize;    // get the height and width of the screen
    GetClientRect(hwnd, &windowsize);

    srcheight = windowsize.bottom;
    srcwidth = windowsize.right;

    if (!newHeight)
        height = windowsize.bottom;
    else
        height = newHeight;

    if (!newWidth)
        width = windowsize.right;
    else
        width = newWidth;

    src.create(height, width, CV_8UC4);

    // create a bitmap
    hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
    bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
    bi.biWidth = width;
    bi.biHeight = -height;  //this is the line that makes it draw upside down or not
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    // use the previously created device context with the bitmap
    SelectObject(hwindowCompatibleDC, hbwindow);
    // copy from the window device context to the bitmap device context
    StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0,srcwidth,srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
    GetDIBits(hwindowCompatibleDC,hbwindow, 0, height, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

    // cleanup
    DeleteObject (hbwindow);
    DeleteDC(hwindowCompatibleDC);
    ReleaseDC(hwnd, hwindowDC);

    return src;
}
