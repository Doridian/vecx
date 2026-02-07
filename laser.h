#ifndef __LASER_H
#define __LASER_H

#include "graphics.h"

typedef struct {
    long rate;
    Point position;
    unsigned char color;
} LaserState;

extern LaserState LaserStateZero;

void LaserInitialize(LaserState *state);
void LaserRenderLine(LaserState *state, Point p0, Point p1, unsigned char color);
void LaserRenderFrame(LaserState *state);

#endif // __LASER_H
