#ifdef _WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include "clockClass.h"

#include <ctime>

#ifdef __unix__

unsigned int GetTickCount() {
    struct timeval t;
    gettimeofday(&t, NULL);

    unsigned long secs = t.tv_sec * 1000;
    secs += (t.tv_usec / 1000);

    return secs;
}

#endif

ClockClass::ClockClass():
	totalTime_(0.0f),
	lastTime_(0.0f),
	paused_(false) 
{
}

ClockClass::~ClockClass()
{
}

void ClockClass::setup()
{
	lastTime_ = GetTickCount() / 1000.0f;
}

float ClockClass::calculateDeltaSeconds()
{
	float currentTime = float(GetTickCount()) / 1000.0f;
    float seconds = float(currentTime - lastTime_);
    lastTime_ = currentTime;

	if(seconds > 0.15f)
		seconds = 0.15f;

    return seconds;
}

void ClockClass::update(float dtRealSeconds)
{
	if(!paused_)
		totalTime_ += dtRealSeconds;
}

void ClockClass::setPaused(bool paused)
{
	paused_ = paused;
}

bool ClockClass::isPaused()
{
	return paused_;
}
