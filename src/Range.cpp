#include "Range.h"

template<typename T>
Range<T>::Range(T from, T until) : m_from(from), m_until(until)
{

}

template<typename T>
Range<T>::Range(const Range<T>& range) : m_from(range.from()), m_until(range.until())
{

}

template<typename T>
T Range<T>::from() const
{
    return m_from;
}

template<typename T>
T Range<T>::to() const
{
    return m_until - 1;
}

template<typename T>
T Range<T>::until() const
{
    return m_until;
}

template<typename T>
T Range<T>::length() const
{
    return m_until - m_from;
}

template<typename T>
bool Range<T>::containsInside(const T& position) const
{
    return from() < position && position < until();
}

template<typename T>
bool Range<T>::contains(const T& position) const
{
    return from() <= position && position < until();
}

template<typename T>
bool Range<T>::precedes(const T& position) const
{
    return from() < position;
}

template<typename T>
bool Range<T>::between(const T& start, const T& end) const
{
    return start <= from() && until() <= end;
}

template<typename T>
bool Range<T>::between(const Range<T>& range) const
{
    return between(range.from(), range.until());
}

template<typename T>
bool Range<T>::operator==(const Range<T>& range) const
{
    return from() == range.from() && until() == range.until();
}

template<typename T>
bool Range<T>::operator!=(const Range<T>& range) const
{
    return !(*this == range);
}

template<typename T>
Range<T>& Range<T>::operator+=(const T& offset)
{
    m_from += offset;
    m_until += offset;

    return *this;
}

template<typename T>
Range<T>& Range<T>::operator-=(const T& offset)
{
    m_from -= offset;
    m_until -= offset;

    return *this;
}

template<typename T>
Range<T> Range<T>::operator+(const T& offset) const
{
    return Range(from() + offset, until() + offset);
}

template<typename T>
Range<T> Range<T>::operator-(const T& offset) const
{
    return Range(from() - offset, until() - offset);
}

template struct Range<int>;
template struct Range<double>;
template struct Range<float>;
