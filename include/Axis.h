#pragma once

#include <cstdint>
#include <map>

class Axis
{
public:
    Axis(int lower, int middle, int upper, uint16_t count);
    int getValue(int value) const;

private:
    struct RangeInner
    {
        RangeInner(int aLeft, int aRight);
        int left;
        int right;

        bool operator < (const RangeInner &rhs) const; 
    };

    enum Direction
    {
        FORWARD
        , BACKWARD
    };

    void BuildThresholds();

private:
    std::map<RangeInner, int> m_levels;

    int m_lower;
    int m_middle;
    int m_upper;
    uint16_t m_count;

    Direction m_direction;
};
