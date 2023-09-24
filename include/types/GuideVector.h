#pragma once

struct GuideVector
{
    int8_t x0 = 0;
    int8_t y0 = 0;

    int8_t x1 = 0;
    int8_t y1 = 0;

    GuideVector()
    {}

    GuideVector(int8_t aX0, int8_t aY0, int8_t aX1, int8_t aY1)
        : x0{aX0}, y0{aY0}, x1{aX1}, y1{aY1}
    {}
};