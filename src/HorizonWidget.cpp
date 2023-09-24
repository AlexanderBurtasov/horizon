#include "HorizonWidget.h"
// std
#include <cmath>
#include <iostream>
#include <tuple>
// prj
#include "st7789.h"

using namespace std;

namespace
{
int getAngleNormalized(int value);
int getNormalizedSpan(int a0, int a1);
double degreesToRadians(double value);

const int cFovDefault = 90;
}
//---------------------------------------------------------------------------
HorizonWidget::HorizonWidget(TFT_t *tft, int16_t width, int16_t height)
    : m_tft{tft}
    , m_width{width}
    , m_height{height}
    , m_fov{cFovDefault}
{
    //const int cDelta = 180; // 3 items range
    const int cDelta = 16;
    // building color palette
    for (int i = 192; i > 0; i -= cDelta)
    {
        m_skyColors_.push_back(::rgb565_conv(16, i, 192));
        m_gndColors_.push_back(::rgb565_conv(192, i, 16));
    }

    buildRanges();

    ::lcdFillScreen(m_tft, 0);
}
//---------------------------------------------------------------------------
void HorizonWidget::buildRanges()
{
    auto addColorPalette = [this]( const std::vector<uint16_t>& colors, int start, int span ) -> void
    {
        const size_t count = ( colors.size() - 1 ) * 2 + 1;
        const double delta = ((double) span) / count;

        double currentAngle = start;
        int begin = start;
        size_t pos = 0;

        for (size_t i = 0; i < count; ++i)
        {
            //qDebug() << round( currentAngle ) << " " << round(angle + delta) - 1 << endl;
            currentAngle += delta;
            int v2 = round(currentAngle);
            int v3 = v2 - begin - 1;
            if (v3 > 1)
            {
                //qDebug() << begin << " " << v2 - 1 << endl;

                m_ranges[{ begin, v2 - 1}] = colors.at(pos);
                
                begin = v2;

                if (i < colors.size())
                {
                    ++pos;
                }
                else
                {
                    --pos;
                }
                
                if (pos == colors.size())
                {
                    pos -= 2;
                }
            }
        }
    };

    addColorPalette(m_skyColors_, 0, 180);
    addColorPalette(m_gndColors_, 180, 180);
}
//---------------------------------------------------------------------------
void HorizonWidget::paintEvent()
{
    //dummyDraw();
    drawSkyes();
    drawAircraft();
    //drawHints();
}
//---------------------------------------------------------------------------
void HorizonWidget::setPitch(int value)
{
    m_pitch = value;
}
//---------------------------------------------------------------------------
void HorizonWidget::setRoll(int value)
{
    m_roll = value;
}
//---------------------------------------------------------------------------
void HorizonWidget::dummyDraw() const
{
    uint8_t y = 12;
    uint16_t pos = 0;
    uint8_t width = 4;
    const auto &rColors = m_pos % 2 ? m_skyColors_ : m_gndColors_;
    for (uint8_t row = 0; row < 12; ++row)
    {
        uint8_t x = 12;
        for (uint8_t col = 0; col < 12; ++col)
        {
            ::DrawFillRect(m_tft, x, y, width, width, rColors[pos]);
            x += width + 1;
            ++pos;
            if (pos >= rColors.size())
            {
                pos = 0;
            }
        }
        y += width + 1;
    }
    ++m_pos;
}
//---------------------------------------------------------------------------
void HorizonWidget::drawAircraft() const
{
    const vector<QPoint> pathPoints = { {-4, 0}, {-3, 0}, {-2, -1}, {-1, 0}
                                        , {1, 0}, {2, -1}, {3, 0}, {4, 0} };

    const int8_t widthScale = 10, heightScale = 10;
    auto pathToScreen = [&](const QPointD &point) -> QPoint
    {
        return { static_cast<int16_t>(m_width / 2 + (point.x * widthScale))
            , static_cast<int16_t>(m_height / 2 - (point.y * heightScale)) };
    };

    double angle = ::degreesToRadians(::getAngleNormalized(m_roll));
    const double cs = ::cos(angle)
        , sn = ::sin(angle);

    vector<QPoint> screenPoints;
    screenPoints.reserve(pathPoints.size());

    for (const auto &tt : pathPoints)
    {
        screenPoints.push_back(pathToScreen({tt.x * cs - tt.y * sn, tt.y * cs + tt.x * sn}));
    }

    /*const */QPoint &first = screenPoints[0];

    for (uint16_t i = 1; i < screenPoints.size(); ++i)
    {
        /*const */QPoint &second = screenPoints[i];
        ::DrawLine(m_tft, first.x, first.y, second.x, second.y, YELLOW);
        first = screenPoints[i];
    }
}
//---------------------------------------------------------------------------
void HorizonWidget::drawSkyes() const
{
    // skyes
    int pitch = ::getAngleNormalized( m_pitch );
    auto startIt = m_ranges.find( { pitch, pitch } );

    // upper
    auto it = startIt;
    while (true)
    {
        ++it;
        if (it == m_ranges.end())
        {
            it = m_ranges.begin();
        }
        int posLower = getCoordForAngle( it->first.begin() );
        int posUpper = getCoordForAngle( it->first.end() + 1 );

        //::lcdDrawFillRect(m_tft, 0, posUpper, m_width, posLower, it->second);
        ::DrawFillRect(m_tft, 0, posUpper, m_width, posLower - posUpper, it->second);
        if (m_ranges.end() == it || posUpper < 0)
        {
            break;
        }
    }

    // bottom
    it = startIt;
    while (true)
    {
        int posLower = getCoordForAngle( it->first.begin() );
        int posUpper = getCoordForAngle( it->first.end() + 1 );

        //::lcdDrawFillRect(m_tft, 0, posUpper, m_width, posLower, it->second);
        ::DrawFillRect(m_tft, 0, posUpper, m_width, posLower - posUpper, it->second);
        if (posLower >= m_height)
        {
            break;
        }
        if (m_ranges.begin() == it)
        {
            it = m_ranges.end(); --it;
        }
        else
        {
            --it;
        }
    }
}
//---------------------------------------------------------------------------
int HorizonWidget::getCoordForAngle( int angle ) const
{
    const int s1 = ::getAngleNormalized( m_pitch );
    const int s2 = ::getAngleNormalized( angle );

    const int span = ::getNormalizedSpan( s1, s2 );
    const int result = m_height / 2 - span * m_height / ( 2 * m_fov );

    return result;
}
//---------------------------------------------------------------------------
const uint16_t& HorizonWidget::getColorForAngle(int angle) const
{
    auto calcSize = []( const std::vector<uint16_t> &items ) -> size_t
    {
        return 2 * ( items.size() - 1 ) + 1;
    };
    const int tmpAngle = ::getAngleNormalized(angle);

    if (tmpAngle >= 0 && tmpAngle < 180)
    {
        size_t pos = tmpAngle * calcSize(m_skyColors_) / 180;
        if (pos >= m_skyColors_.size())
        {
            pos = calcSize(m_skyColors_) - pos;
        }

        return m_skyColors_[pos];
    }

    size_t pos = (360 - tmpAngle - 1)* calcSize(m_gndColors_) / 180;
    if (pos >= m_gndColors_.size())
    {
        pos = calcSize( m_gndColors_ ) - pos;
    }

    return m_gndColors_[pos];
}
//---------------------------------------------------------------------------
void HorizonWidget::drawHints() const
{
//    painter.setPen(Qt::green);
//    QFont drawFont{"Arial", 12};
//    painter.setFont(drawFont);
//    char buf[256];
//
//    {
//        sprintf_s(buf, sizeof(buf), "%3d", m_pitch);
//        auto [textWidth, textHeight] = calculateBounds(drawFont, buf);
//        QRect drawArea{ 10, m_height / 2 - textHeight / 2
//                    , textWidth, textHeight };
//        painter.drawText(drawArea, Qt::AlignCenter, buf);
//    }

//    {
//        sprintf_s(buf, sizeof(buf), "%3d", m_roll);
//        auto [textWidth, textHeight] = calculateBounds(drawFont, buf);
//        QRect drawArea{ m_width - textWidth - 10, m_height - textHeight - 10
//                    , textWidth, textHeight };
//        painter.drawText(drawArea, Qt::AlignCenter, buf);
//    }
}
//---------------------------------------------------------------------------
namespace
{
//---------------------------------------------------------------------------
int getAngleNormalized(int value)
{
    int result = value % 360;

    if (result < 0)
    {
        return 360 + result;
    }

    return result;
}
//---------------------------------------------------------------------------
int getNormalizedSpan(int from, int to)
{
    int delta = to - from;
    if (delta > 180)
    {
        delta -= 360;
    }
    else if (delta < -180)
    {
        delta = 360 + delta;
    }

    return delta;
}
//---------------------------------------------------------------------------
double degreesToRadians(double value)
{
    return value / 180 * 3.14159265358979323846;
}
// //---------------------------------------------------------------------------
// double boundedTan(double value)
// {
//     double div = cos(value);
//     if (::fabs(div) < 0.0000001)
//     {
//         div = div < 0 ? -0.00000001 : 0.00000001;
//     }
//     return sin(value) / div;
// }
//---------------------------------------------------------------------------
} // namespace
//---------------------------------------------------------------------------
