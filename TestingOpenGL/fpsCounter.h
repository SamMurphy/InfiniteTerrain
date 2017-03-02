#pragma once
class fpsCounter
{
public:
	fpsCounter();
	~fpsCounter();

	void fpsinit();
	void fpsthink();
	int avg_fps;
private:
	int frametimes[10];
	int frametimelast;
	int framecount;
	float framespersecond;
};

