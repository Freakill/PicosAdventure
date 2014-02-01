#ifndef _CLOCK_CLASS_H_
#define _CLOCK_CLASS_H_

#include <windows.h>
#include <ctime>

class ClockClass 
{
	public:
		ClockClass();
		~ClockClass();

		void setup();
		float calculateDeltaSeconds();
		void update(float dtRealSeconds);
		void setPaused(bool paused);
		bool isPaused();
		
	private:
		long double totalTime_;
		float lastTime_;
		bool paused_;

};

#endif //_CLOCK_CLASS_H_
