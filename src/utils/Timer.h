//////////////////////////////////////////////////////////////////////////
//Name		:	Timer.h
//Function	:	A simple timing class 
//Author	:	Junyan Zhu, Tsinghua University
//Note		:   It's simple timing class, you could define the timer class at the beginning of the function. 
//////////////////////////////////////////////////////////////////////////

#ifndef TIMER_H
#define TIMER_H
#include "../common.h"

class CTimer {
public:
	//End the timing when destroyed 
	CTimer();
	//Define the timer class without a name
	~CTimer();
	//Define the timer class with a specific name 
	CTimer(string timerName, bool isTiming = true);
    // get time
    double Time();

private:
	string m_name;
	clock_t m_start;
	clock_t m_end;
    double m_start_mp;
    double m_end_mp;
	bool m_isTime; 
};
#endif //TIMER_H
