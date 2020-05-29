#include "numbers.h"

int numbers::clamp(int source, int min, int max)
{
    if (source > max)
        return max;

    if (source < min)
        return min;

    return source;
}
