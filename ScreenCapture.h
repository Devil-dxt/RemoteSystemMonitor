#pragma once
#include <vector>
#include <Windows.h>

class ScreenCapturer {
public:
    std::vector<BYTE> CaptureScreen(int quality);

private:
    HBITMAP CreateScreenBitmap(HDC hdc, int& width, int& height);
    std::vector<BYTE> ConvertToJpeg(HBITMAP hBitmap, int quality);
};