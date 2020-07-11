#ifndef RANGE_H
#define RANGE_H

template<typename  T>
class Range
{
    public:
        //! Constructs an invalid range.
        Range();
        //! Constructs a range between from and until.
        Range(T from, T until);
        //! Constructs a copy of the specified range.
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
        //! Returns true if the given position is fully inside this range.
        //! A position is considered "inside" if it is in-between the range's
        //! start and end index, excluding the start and end indices themselves.
        bool containsInside(const T position) const;
        //! Returns true if the given position is contained by this range.
        bool contains(const T position) const;
        //! Returns true if this range precedes the given position, that is
        //! if the position comes after the range's start.
        bool precedes(const T position) const;
        //! Returns true if this range lies between the specified start and end points.
        bool between(const T start, const T end) const;
        //! Returns true if this range lies between the specified range.
        bool between(const Range<T>& range) const;
        //! Returns true if this range intersects with the range denoted by the
        //! specified start and end points.
        bool intersects(const T start, const T end) const;
        //! Returns true if this range intersects with the specified range.
        bool intersects(const Range<T>& range) const;
        //! If this range intersects with the range denoted by the specified
        //! start and end points, returns a new Range of the overlap between
        //! the two, else returns an invalid Range.
        Range<T> intersect(const T start, const T end) const;
        //! If this range intersects with the specified range, returns a new
        //! Range of the overlap between the two, else returns an invalid Range.
        Range<T> intersect(const Range<T>& range) const;

        //! Returns true if the Range is a valid range. A Range is valid if and
        //! only if the from value lies before the until value.
        bool isValid() const;

        bool operator==(const Range& range) const;
        bool operator!=(const Range& range) const;
        Range& operator+=(const T offset);
        Range& operator-=(const T offset);
        Range operator+(const T offset) const;
        Range operator-(const T offset) const;

    private:
        T m_from;
        T m_until;
};

#endif // RANGE_H
