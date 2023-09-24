#pragma once

template <typename T>
class RangeInner
{
public:
  RangeInner(const T &begin, const T &end)
  {
    if (begin < end)
    {
      m_begin = begin;
      m_end = end;
    }
    else
    {
      m_begin = end;
      m_end = begin;
    }
  }

  const T &begin() const
  {
    return m_begin;
  }

  const T &end() const
  {
      return m_end;
  }

  bool operator < (const RangeInner<T> &rhs) const
  {
    return m_end < rhs.m_begin && m_begin < rhs.m_begin;
  }

private:
  T m_begin;
  T m_end;
};

typedef RangeInner<int> RangeInnerI;
