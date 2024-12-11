#pragma once

#include <Windows.h>

struct Buffer {
	HDC hdc;
	HWND hWnd;
	HBITMAP bgBitmap;
	RECT screenBufferRect;
};

Buffer* CreateScreenBuffer(HWND hWnd);

Buffer* CreateScreenBuffer(HWND hWnd, RECT rect);

void ReleaseScreenBuffer(Buffer* buffer);

void ResizeScreenBuffer(Buffer** buffer);

void RefreshScreenBuffer(Buffer* buffer);

void DrawScreenBuffer(Buffer* buffer, HDC hdcDest);

void ModifyPixels(Buffer* buffer, RECT selectionRect, int incRed, int incGreen, int incBlue);