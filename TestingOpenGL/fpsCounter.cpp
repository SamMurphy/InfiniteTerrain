#include "fpsCounter.h"
#include <SDL/SDL.h>


fpsCounter::fpsCounter()
{
}


fpsCounter::~fpsCounter()
{
}

void fpsCounter::fpsinit() {

	// Set all frame times to 0ms.
	for (int i : frametimes) i = 0;
	framecount = 0;
	framespersecond = 0;
	frametimelast = SDL_GetTicks();
}

void fpsCounter::fpsthink() {

	int frametimesindex;
	int getticks;
	int count;
	int i;

	frametimesindex = framecount % 10;
	getticks = SDL_GetTicks();
	frametimes[frametimesindex] = getticks - frametimelast;
	frametimelast = getticks;
	framecount++;

	if (framecount < 10)
		count = framecount;
	else
		count = 10;

	// add up all the values and divide to get the average frame time.
	framespersecond = 0;
	for (i = 0; i < count; i++) {
		framespersecond += frametimes[i];
	}
	framespersecond /= count;
	framespersecond = 1000.f / framespersecond;
	avg_fps = (int)framespersecond;
}
