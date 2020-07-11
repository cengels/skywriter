#include "numbers.h"

int numbers::clamp(const int source, const int min, const int max)
{
    if (source > max)
        return max;

    if (source < min)
        return min;

    return source;
}
