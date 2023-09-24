#pragma once
//std
#include <array>
#include <vector>
#include <map>
//
#include "RangeInner.h"

template <typename T>
struct Point
{
    T x;
    T y;
};

using QPoint = Point<int16_t>;
using QPointD = Point<double>;

struct TFT_t;

class HorizonWidget
{
public:
    HorizonWidget(TFT_t *tft, int16_t width, int16_t height);
    void paintEvent();

    void setPitch(int value);
    void setRoll(int value);

private:
    void dummyDraw() const;
    void drawSkyes() const;
    void drawMidline() const;
    void drawMidCircle() const;
    void drawHints() const;
    void drawAircraft() const;

    static bool isPerpendicular(double rollAngle);
    const uint16_t &getColorForAngle(int angle) const;
    int getCoordForAngle(int angle) const;
    
    void buildRanges();

private:
    TFT_t *m_tft = nullptr;    
    std::vector<uint16_t> m_skyColors_;
    std::vector<uint16_t> m_gndColors_;
    int16_t m_width;
    int16_t m_height;
    int m_pitch = 0.;
    int m_roll = 0.;

    std::map<RangeInnerI, uint16_t> m_ranges;
    int m_fov;

    mutable size_t m_pos = 0;
};
