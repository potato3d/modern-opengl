#pragma once
#include <chrono>

class Timer
{
public:
	static int get_resolution();
	Timer();
	void restart();
	double sec();
	double msec();
	double usec();
	double nsec();

private:
	typedef typename std::conditional<std::chrono::high_resolution_clock::is_steady,
	                                  std::chrono::high_resolution_clock,
	                                  std::chrono::steady_clock>::type t_clock;
	t_clock::time_point _start;
};

int Timer::get_resolution()
{
	return t_clock::period::den;
}

Timer::Timer()
{
	restart();
}

void Timer::restart()
{
	_start = t_clock::now();
}

double Timer::sec()
{
	return std::chrono::duration<double>(t_clock::now() - _start).count();
}

double Timer::msec()
{
	return std::chrono::duration<double, std::milli>(t_clock::now() - _start).count();
}

double Timer::usec()
{
	return std::chrono::duration<double, std::micro>(t_clock::now() - _start).count();
}

double Timer::nsec()
{
	return std::chrono::duration<double, std::nano>(t_clock::now() - _start).count();
}
