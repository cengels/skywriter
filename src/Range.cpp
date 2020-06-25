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
Range<T> Range<T>::offset(const T& offset) const
{
    return Range(from() + offset, until() + offset);
}

template struct Range<int>;
template struct Range<double>;
template struct Range<float>;
