#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>

#include "helios_dac/sdk/cpp/HeliosDac.h"
#include "laser.h"
#include "vecx.h"

#define LASER_FULL_POWER 0x7F
#define LASER_MAX_COORDINATE 0xFFF
#define LASER_CYCLES_TO_OFF 4
#define LASER_CYCLES_TO_ON 4
#define LASER_CYCLES_TO_STABLE_ON 2
#define LASER_CYCLES_TO_STABLE_OFF 1
#define LASER_MAX_DISTANCE_PER_CYCLE_ON 5
#define LASER_MAX_DISTANCE_PER_CYCLE_OFF 10

static HeliosDac helios;
// We target 50 Hz, we use a rate of 20 Kpts, so we can send at most 400 points per frame...
#define FRAME_SIZE (128 * 1024)
static HeliosPoint frame[FRAME_SIZE];
static volatile int frameIndex = 0;
static int frameOver = 0;
static double laserScaleFactor = 1.0;

LaserState LaserStateZero = { 0 };

void RenderPoint(LaserState *state, Point point, unsigned char color)
{
    if (frameIndex >= FRAME_SIZE) {
        frameOver++;
        return;
    }
    frame[frameIndex].x = point.x;
    frame[frameIndex].y = point.y;
    frame[frameIndex].r = 0;
    frame[frameIndex].g = 0;
    frame[frameIndex].b = (((double)color) / (double)255.0) * (double)LASER_FULL_POWER;
    frameIndex++;
}

void LaserRenderFrame(LaserState *state)
{
    if (frameIndex == 0) {
        return;
    }
    if (helios.GetStatus(0) == 1) {
        helios.WriteFrame(0, state->rate, HELIOS_FLAGS_SINGLE_MODE|HELIOS_FLAGS_DONT_BLOCK, frame, frameIndex);
        if (frameOver > 0) {
            fprintf(stderr, "Warning: frame overrun! %d frames were lost.\n", frameOver);
        }   
    }
    frameIndex = 0;
    frameOver = 0;
}

void LaserStateSetRate(LaserState *state, long rate)
{
    if (state->rate == rate) {
        return;
    }
    state->rate = rate;
}

void RenderPause(LaserState *state, long cycles)
{
    int i;
    for (i = 0; i < cycles; i++) {
        RenderPoint(state, state->position, state->color);
    }
}

void LaserStateSetOn(LaserState *state, unsigned char color)
{
    int i;

    if ((state->color > 0) == (color > 0)) {
        return;
    }

    state->color = color;
    if (color > 0) {
        RenderPause(state, LASER_CYCLES_TO_ON);
    } else {
        RenderPause(state, LASER_CYCLES_TO_OFF);
    }
}

void LaserStateSetPosition(LaserState *state, Point position)
{
    int i;

    // Look up the maximum move distance for current state.
    float max_distance = (state->color > 0) ? LASER_MAX_DISTANCE_PER_CYCLE_ON : LASER_MAX_DISTANCE_PER_CYCLE_OFF;

    // Determine the distance to the point.
    double distance = DistanceFromPointToPoint(state->position, position);
    float steps = ceil(distance / max_distance);
    double moveX = (position.x - state->position.x) / steps;
    double moveY = (position.y - state->position.y) / steps;

    for (i = 0; i < steps; i++) {
        Point intermediate = PointZero;
        intermediate.x = state->position.x + (i * moveX);
        intermediate.y = state->position.y + (i * moveY);
        RenderPoint(state, intermediate, state->color);
    }

    (*state).position = position;
    RenderPause(state, (state->color > 0) ? LASER_CYCLES_TO_STABLE_ON : LASER_CYCLES_TO_STABLE_OFF);
}

void LaserRenderLine(LaserState *state, Point p0, Point p1, unsigned char color)
{
    p0 = PointScale(p0, laserScaleFactor);
    p1 = PointScale(p1, laserScaleFactor);

    // Switch the laser off if the line is discontinuous.
    if (!PointEqualToPoint(state->position, p0, 1)) {
        LaserStateSetOn(state, 0);
        LaserStateSetPosition(state, p0);
        LaserStateSetOn(state, color);
    }

    // Draw the line.
    LaserStateSetPosition(state, p1);
}

void LaserInitialize(LaserState *state)
{
    LaserStateSetRate(state, 20000);

    laserScaleFactor = (double)LASER_MAX_COORDINATE / (double)std::max(ALG_MAX_Y, ALG_MAX_X);

    int numDevices = helios.OpenDevices();
	printf("Found %d DACs:\n", numDevices);
	for (int j = 0; j < numDevices; j++)
	{
		char name[32];
		if (helios.GetName(j,   name) == HELIOS_SUCCESS)
			printf("- %s: type: %s, FW: %d\n", name, helios.GetIsUsb(j) ? "USB" : "IDN/Network", helios.GetFirmwareVersion(j));
		else
			printf("- (unknown dac): USB?: %d, FW %d\n", helios.GetIsUsb(j), helios.GetFirmwareVersion(j));
	}
}

