#include <math.h>

#include "graphics.h"

Point PointZero = { 0 };

Point PointMake(double x, double y)
{
	Point point = { 0 };
	point.x = x;
	point.y = y;
	return point;
}

Point PointScale(Point point, double scale)
{
	return PointMake(point.x * scale, point.y * scale);
}

bool DoubleEqualToDouble(double a, double b, double epsilon)
{
    return fabs(a - b) < epsilon;
}

bool PointEqualToPoint(Point p0, Point p1, double epsilon)
{
    return DoubleEqualToDouble(p0.x, p1.x, epsilon) &&
           DoubleEqualToDouble(p0.y, p1.y, epsilon);
}

float DistanceFromPointToPoint(Point p0, Point p1)
{
    double x = p1.x - p0.x;
    double y = p1.y - p0.y;

    return sqrt((x * x) + (y * y));
}
