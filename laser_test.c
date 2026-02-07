#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "laser.h"

#define SCALE 100.0

int main()
{
    Point p0 = PointScale(PointMake(10, 10), SCALE);
    Point p1 = PointScale(PointMake(10, 20), SCALE);
    Point p2 = PointScale(PointMake(20, 20), SCALE);
    Point p3 = PointScale(PointMake(20, 10), SCALE);

    Point p4 = PointScale(PointMake(30, 30), SCALE);
    Point p5 = PointScale(PointMake(30, 40), SCALE);
    Point p6 = PointScale(PointMake(40, 40), SCALE);
    Point p7 = PointScale(PointMake(40, 30), SCALE);

    LaserState state = LaserStateZero;

    LaserInitialize(&state);

    while (1) {
        LaserRenderLine(&state, p0, p1);
        LaserRenderLine(&state, p1, p2);
        LaserRenderLine(&state, p2, p3);
        LaserRenderLine(&state, p3, p0);

        LaserRenderLine(&state, p4, p5);
        LaserRenderLine(&state, p5, p6);
        LaserRenderLine(&state, p6, p7);
        LaserRenderLine(&state, p7, p4);

        LaserRenderFrame(&state);

        usleep(1000);
    }
}
