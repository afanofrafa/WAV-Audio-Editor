#include "Buffer.h"

Buffer* CreateScreenBuffer(HWND hWnd) {
    Buffer* buffer = (Buffer*)malloc(sizeof(Buffer));

    buffer->hWnd = hWnd;
    HDC windowDC;
    GetClientRect(hWnd, &buffer->screenBufferRect);
    windowDC = GetDC(hWnd);
    buffer->hdc = CreateCompatibleDC(windowDC);
    SaveDC(buffer->hdc);
    buffer->bgBitmap = CreateCompatibleBitmap(windowDC,
        buffer->screenBufferRect.right - buffer->screenBufferRect.left,
        buffer->screenBufferRect.bottom - buffer->screenBufferRect.top);
    SelectObject(buffer->hdc, buffer->bgBitmap);
    SetBkMode(buffer->hdc, TRANSPARENT);
    ReleaseDC(hWnd, windowDC);

    return buffer;
}

Buffer* CreateScreenBuffer(HWND hWnd, RECT rect)
{
    Buffer* buffer = (Buffer*)malloc(sizeof(Buffer));

    buffer->hWnd = hWnd;
    HDC windowDC;
    buffer->screenBufferRect = rect;
    windowDC = GetDC(hWnd);
    buffer->hdc = CreateCompatibleDC(windowDC);
    SaveDC(buffer->hdc);
    buffer->bgBitmap = CreateCompatibleBitmap(windowDC,
        buffer->screenBufferRect.right - buffer->screenBufferRect.left,
        buffer->screenBufferRect.bottom - buffer->screenBufferRect.top);
    SelectObject(buffer->hdc, buffer->bgBitmap);
    SetBkMode(buffer->hdc, TRANSPARENT);
    ReleaseDC(hWnd, windowDC);

    return buffer;
}

void ReleaseScreenBuffer(Buffer* buffer) {
    if (buffer != NULL) {
        if (buffer->hdc != NULL)
            RestoreDC(buffer->hdc, -1);
        DeleteObject(buffer->bgBitmap);
        DeleteDC(buffer->hdc);
        free(buffer);
    }
}

void ResizeScreenBuffer(Buffer** buffer) {
    HWND hWnd = (*buffer)->hWnd;
    ReleaseScreenBuffer(*buffer);
    *buffer = CreateScreenBuffer(hWnd);
}

void RefreshScreenBuffer(Buffer* buffer) {
    FillRect(buffer->hdc, &buffer->screenBufferRect, (HBRUSH)GetStockObject(WHITE_BRUSH));
}

void DrawScreenBuffer(Buffer* buffer, HDC hdcDest) {
    BitBlt(hdcDest, 0, 0, 
        buffer->screenBufferRect.right - buffer->screenBufferRect.left,
        buffer->screenBufferRect.bottom - buffer->screenBufferRect.top,
        buffer->hdc, 0, 0, SRCCOPY
    );
}

void ModifyPixels(Buffer* buffer, RECT selectionRect, int incRed, int incGreen, int incBlue) {
    // Получаем данные битмапа
    BITMAP bitmap;
    GetObject(buffer->bgBitmap, sizeof(bitmap), &bitmap);

    // Создаем массив для хранения пикселей
    int width = bitmap.bmWidth;
    int height = bitmap.bmHeight;
    int bytesPerPixel = 4; // Для 32-битного цвета
    int imageSize = width * height * bytesPerPixel;

    // Получаем доступ к пикселям
    BITMAPINFO bmpInfo;
    ZeroMemory(&bmpInfo, sizeof(bmpInfo));
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = width;
    bmpInfo.bmiHeader.biHeight = height;
    bmpInfo.bmiHeader.biPlanes = 1;
    bmpInfo.bmiHeader.biBitCount = 32;
    bmpInfo.bmiHeader.biCompression = BI_RGB;

    // Временный буфер для хранения пикселей
    BYTE* pixelData = new BYTE[imageSize];

    // Получаем данные битмапа
    GetDIBits(buffer->hdc, buffer->bgBitmap, 0, height, pixelData, &bmpInfo, DIB_RGB_COLORS);

    // Изменяем цвет пикселей в области выделения
    for (int y = selectionRect.top; y < selectionRect.bottom; ++y) {
        for (int x = selectionRect.left; x < selectionRect.right; ++x) {
            int index = (y * width + x) * bytesPerPixel;

            // Получаем текущий цвет пикселя
            BYTE r = pixelData[index + 2]; // Красный
            BYTE g = pixelData[index + 1]; // Зеленый
            BYTE b = pixelData[index];     // Синий

            // Увеличиваем цветовые компоненты
            pixelData[index + 2] = (r + incRed) % 256; // Красный
            pixelData[index + 1] = (g + incGreen) % 256; // Зеленый
            pixelData[index] = (b + incBlue) % 256;     // Синий
        }
    }

    // Загружаем измененные данные обратно в битмап
    SetDIBits(buffer->hdc, buffer->bgBitmap, 0, height, pixelData, &bmpInfo, DIB_RGB_COLORS);

    // Освобождаем временный буфер
    delete[] pixelData;
}