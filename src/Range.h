#ifndef RANGE_H
#define RANGE_H

template<typename  T>
class Range
{
    public:
        Range(T from, T until);
        Range(const Range& range);

        //! The inclusive start of the range.
        T from() const;
        //! The final value in the range (inclusive).
        T to() const;
        //! The first value outside the range (exclusive). This is the value that
        //! was passed as the second constructor argument.
        T until() const;
        //! The difference between from() and until().
        T length() const;

        //! Creates a new Range that is offset from this one.
        Range offset(const T& offset) const;

        bool operator==(const Range& range) const;
        bool operator!=(const Range& range) const;

    private:
        T m_from;
        T m_until;
};

#endif // RANGE_H
