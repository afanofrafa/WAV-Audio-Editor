#include "ButtonsDrawings.h"
#define PI 3.14159265358979323846
void DrawLoop(HDC hdc) {
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(81, 124, 140));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    std::vector<POINT> points = {
        {10, 10},
        {40, 10},
        {40, 32},
        {37, 32},
        {37, 13},
        {10, 13},
    };
    Polygon(hdc, points.data(), 6);
    // Угол стрелки (в радианах)
    double angle = PI / 2;

    // Левое и правое крыло стрелки
    POINT leftWing = {
        38 - (int)(12 * cos(angle - PI / 6)),
        32 - (int)(12 * sin(angle - PI / 6))
    };
    POINT rightWing = {
        39 - (int)(12 * cos(angle + PI / 6)),
        32 - (int)(12 * sin(angle + PI / 6))
    };
    points.clear();
    points.push_back({ 38, 29 });
    points.push_back(leftWing);
    points.push_back({ leftWing.x, leftWing.y + 5 });
    points.push_back({ 38, 34 });
    points.push_back({ 39, 34 });
    points.push_back({ rightWing.x, rightWing.y + 5 });
    points.push_back(rightWing);
    points.push_back({ 39, 29 });
    Polygon(hdc, points.data(), 8);
    points.clear();
    points = {
        {40, 40},
        {10, 40},
        {10, 18},
        {13, 18},
        {13, 37},
        {40, 37},
    };
    Polygon(hdc, points.data(), 6);
    // Угол стрелки (в радианах)
    angle = 3 * PI / 2;

    // Левое и правое крыло стрелки
    leftWing = {
        12 - (int)(12 * cos(angle - PI / 6)),
        18 - (int)(12 * sin(angle - PI / 6))
    };
    rightWing = {
        11 - (int)(12 * cos(angle + PI / 6)),
        18 - (int)(12 * sin(angle + PI / 6))
    };
    points.clear();
    points.push_back({ 12, 21 });
    points.push_back(leftWing);
    points.push_back({ leftWing.x, leftWing.y - 5 });
    points.push_back({ 12, 16 });
    points.push_back({ 11, 16 });
    points.push_back({ rightWing.x, rightWing.y - 5 });
    points.push_back(rightWing);
    points.push_back({ 11, 21 });
    Polygon(hdc, points.data(), 8);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}
void DrawStart(HDC hdc, COLORREF colorBrush) {
    HBRUSH hBrush = CreateSolidBrush(colorBrush);
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(81, 124, 140));

    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    std::vector<POINT> trianglePoints = {
        {40, 25},
        {10, 10},
        {10, 40},
        {40, 25},
    };

    Polygon(hdc, trianglePoints.data(), 4);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}
void DrawFinish(HDC hdc) {
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(81, 124, 140));

    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    std::vector<POINT> rectPoints = {
        {10, 10},
        {40, 10},
        {40, 40},
        {10, 40},
        {10, 10},
    };
    Polygon(hdc, rectPoints.data(), 5);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}
void DrawPause(HDC hdc) {
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(81, 124, 140));

    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    std::vector<POINT> rectPoints = {
        {10, 10},
        {20, 10},
        {20, 40},
        {10, 40},
        {10, 10},
    };
    Polygon(hdc, rectPoints.data(), 5);

    rectPoints = {
        {30, 10},
        {40, 10},
        {40, 40},
        {30, 40},
        {30, 10},
    };
    Polygon(hdc, rectPoints.data(), 5);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}
void DrawRecord(HDC hdc) {
    HBRUSH hBrush = CreateSolidBrush(RGB(188, 0, 0));
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(206, 188, 214));
    SelectObject(hdc, hBrush);
    SelectObject(hdc, hPen);

    Ellipse(hdc, 10, 10, 40, 40);

    DeleteObject(hBrush);
    DeleteObject(hPen);
}
void DrawMoveToStart(HDC hdc) {
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(81, 124, 140));

    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    std::vector<POINT> rectPoints = {
        {7, 10},
        {7, 40},
        {10, 40},
        {10, 10},
        {7, 10},
    };
    std::vector<POINT> trianglePoints = {
        {10, 25},
        {40, 10},
        {40, 40},
        {10, 25},
    };

    Polygon(hdc, rectPoints.data(), 5);
    Polygon(hdc, trianglePoints.data(), 4);

    // Восстанавливаем исходные объекты и удаляем созданные кисть и карандаш
    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}
void DrawMoveToEnd(HDC hdc) {
    HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(81, 124, 140));

    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    std::vector<POINT> rectPoints = {
        {43, 10},
        {43, 40},
        {40, 40},
        {40, 10},
        {43, 10},
    };

    std::vector<POINT> trianglePoints = {
        {40, 25},
        {10, 10},
        {10, 40},
        {40, 25},
    };

    Polygon(hdc, rectPoints.data(), 5);
    Polygon(hdc, trianglePoints.data(), 4);

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}