#include "SingleOrRandom.h"

float randomFloat()
{
    return (float)(rand()) / (float)(RAND_MAX);
}

int randomInt(int a, int b)
{
    if (a > b)
        return randomInt(b, a);
    if (a == b)
        return a;
    return a + (rand() % (b - a));
}

float randomFloat(float a, float b)
{
    if (a > b)
        return randomFloat(b, a);
    if (a == b)
        return a;

    // take out a lot of the decimal part, get the random int and the "divide" everything to normal
    return ((float)randomInt(a * 1000.0f, b * 1000.0f) * 0.001f);
}
