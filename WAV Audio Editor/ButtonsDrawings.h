#pragma once
#include <windows.h>
#include <vector>
void DrawLoop(HDC hdc);
void DrawStart(HDC hdc, COLORREF colorBrush);
void DrawFinish(HDC hdc);
void DrawPause(HDC hdc);
void DrawRecord(HDC hdc);
void DrawMoveToStart(HDC hdc);
void DrawMoveToEnd(HDC hdc);