#include "ScreenCapture.h"
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

struct GdiplusInitializer {
    GdiplusInitializer() {
        GdiplusStartupInput input;
        GdiplusStartup(&token, &input, NULL);
    }
    ~GdiplusInitializer() { GdiplusShutdown(token); }
    ULONG_PTR token;
} g_gdiplusInitializer;

// ��ȡͼ��������� CLSID�����ʶ����
INT GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT num = 0;          // ����������
    UINT size = 0;         // ��������Ϣ��С

    ImageCodecInfo* pImageCodecInfo = NULL;
    GetImageEncodersSize(&num, &size);

    if (size == 0) return -1;  // û���ҵ�������

    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if (pImageCodecInfo == NULL) return -1;  // �ڴ����ʧ��

    GetImageEncoders(num, size, pImageCodecInfo);

    for (UINT j = 0; j < num; ++j) {
        if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // ���ر���������
        }
    }

    free(pImageCodecInfo);
    return -1;  // δ�ҵ�ƥ��ı�����
}

std::vector<BYTE> ScreenCapturer::CaptureScreen(int quality) {
    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP hBitmap = CreateScreenBitmap(hdcScreen, width, height);
    SelectObject(hdcMem, hBitmap);
    BitBlt(hdcMem, 0, 0, width, height, hdcScreen, 0, 0, SRCCOPY);

    auto jpegData = ConvertToJpeg(hBitmap, quality);

    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);

    return jpegData;
}

HBITMAP ScreenCapturer::CreateScreenBitmap(HDC hdc, int& width, int& height) {
    return CreateCompatibleBitmap(hdc, width, height);
}

std::vector<BYTE> ScreenCapturer::ConvertToJpeg(HBITMAP hBitmap, int quality) {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    Bitmap bitmap(hBitmap, NULL);
    CLSID clsid;
    GetEncoderClsid(L"image/jpeg", &clsid);

    EncoderParameters encoderParams;
    encoderParams.Count = 1;
    encoderParams.Parameter[0].Guid = EncoderQuality;
    encoderParams.Parameter[0].Type = EncoderParameterValueTypeLong;
    encoderParams.Parameter[0].NumberOfValues = 1;
    ULONG qualityValue = quality;
    encoderParams.Parameter[0].Value = &qualityValue;

    IStream* pStream = NULL;
    CreateStreamOnHGlobal(NULL, TRUE, &pStream);
    bitmap.Save(pStream, &clsid, &encoderParams);

    STATSTG stat;
    pStream->Stat(&stat, STATFLAG_NONAME);
    std::vector<BYTE> data(stat.cbSize.LowPart);
    LARGE_INTEGER li{ 0 };
    pStream->Seek(li, STREAM_SEEK_SET, NULL);
    pStream->Read(data.data(), stat.cbSize.LowPart, NULL);

    pStream->Release();
    GdiplusShutdown(gdiplusToken);
    return data;
}