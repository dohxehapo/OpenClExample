#pragma once

#ifndef TIMER_H
#define TIMER_H


#include <Windows.h>


// Simple class that counts the elapsed time in seconds. It's accuracy depends on the system performance counter frequency
class Timer
{
public:


	// Default ctor with the system performance counter frequency check
	Timer()
	{
		QueryPerformanceFrequency((LARGE_INTEGER *)&m_freq);
	}

	// Default dtor
	virtual ~Timer() { }


	// Starts the new timer
	void Start()
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&m_timeStart);
	}

	// Returns the time elapsed since the timer start, the timer doesn't stop
	double Check()
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&m_time);
		return (double)(m_time - m_timeStart) / (double)m_freq;
	}

	// Returns the time elapsed since the timer start and restarts the timer
	double Restart()
	{
		QueryPerformanceCounter((LARGE_INTEGER *)&m_time);
		double dt = (double)(m_time - m_timeStart) / (double)m_freq;
		m_timeStart = m_time;
		return dt;
	}

private:

	__int64 m_timeStart;	// Stores start time
	__int64 m_time;			// Used for calculations
	__int64 m_freq;			// Stores performance counter frequency, used in calculations

};


#endif // TIMER_H
