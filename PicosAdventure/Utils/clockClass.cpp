#include "clockClass.h"

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
