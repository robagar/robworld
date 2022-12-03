#include "../StdAfx.h"

const double pi = 3.14159265358979323846264338327950288419716939937510;

// Converts an angle measured in degrees to radians
double deg2rad(double d)
{
	return (d * pi) / 180.0;
}

// Converts an angle measured in radians to degrees
double rad2deg(double r)
{
	return (r * 180.0) / pi;
}

// Rounds floating point to n decimal places
double round(double d, int n)
{
    int modifier = 1;
    for (int i = 0; i < n; ++i)
        modifier *= 10;

    if(d < 0.0)
        return (ceil(d * modifier - 0.5) / modifier);
    return (floor(d * modifier + 0.5) / modifier);
} 

