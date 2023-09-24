//
#include "Axis.h"
//
#include <math.h>

using namespace std;

Axis::Axis(int lower, int middle, int upper, uint16_t count)
    : m_lower{ lower }
    , m_middle{ middle }
    , m_upper{ upper }
    , m_count{ count }
{
    m_direction = m_lower < m_upper ? FORWARD : BACKWARD;
    if (BACKWARD == m_direction)
    {
        std::swap(m_lower, m_upper);
    }
    BuildThresholds();
}

int Axis::getValue(int value) const
{
    const auto it = m_levels.find({ value, value });
    if (m_levels.end() == it)
    {
        if (FORWARD == m_direction)
        {
            if (value < m_lower)
                return -1 * m_count;
            else if (value > m_upper)
                return m_count;

            return 0;
        }
        else
        {
            if (value < m_lower)
                return m_count;
            else if (value > m_upper)
                return -1 * m_count;

            return 0;
        }
    }

    return it->second;
}

Axis::RangeInner::RangeInner(int aLeft, int aRight) : left{ aLeft }, right{ aRight }
{
    if (left > right)
        std::swap(left, right);
}

bool Axis::RangeInner::operator < (const Axis::RangeInner &rhs) const
{
    return right < rhs.left;
}

void Axis::BuildThresholds()
{
    int l = (FORWARD == m_direction) ? (-1) * m_count : m_count;
    int d = (FORWARD == m_direction) ? 1 : -1;

    int lastValue = m_lower;

    {
        double deltaL = (m_middle - m_lower) / (m_count + 0.5);

        for (size_t i = 1; i <= m_count; ++i)
        {
            int current = static_cast<int>(::round(m_lower + i * deltaL));
            m_levels[{lastValue, current}] = l;
            lastValue = current + 1;
            l += d;
        }
    }

    {
        double deltaL = (m_upper - m_middle) / (m_count + 0.5);
        double k = 0.5;
        while (k < m_count)
        {
            int current = static_cast<int>(::round(m_middle + k * deltaL));
            m_levels[{lastValue, current}] = l;
            k += 1.;
            lastValue = current + 1;
            l += d;
        }
    }

    m_levels[{lastValue, m_upper}] = l;
}
