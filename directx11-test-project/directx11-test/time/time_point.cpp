#include "stdafx.h"
#include "time_point.h"

using xtest::time::TimePoint;
using xtest::time::TimeSpan;


namespace {
	int64 ReadHighResolutionTimerTicks()
	{
		LARGE_INTEGER count;
		QueryPerformanceCounter(&count);
		return int64(count.QuadPart);
	}
} // unnamed


TimePoint TimePoint::Now()
{
	return TimePoint(TimeSpan(ReadHighResolutionTimerTicks()));
}

TimePoint::TimePoint()
	: m_timeSpan(TimeSpan(int64(0)))
{}

TimePoint::TimePoint(const TimeSpan& timeSpan)
	: m_timeSpan(timeSpan)
{}

TimePoint& TimePoint::operator+=(const TimeSpan& timeSpan)
{
	m_timeSpan += timeSpan;
	return *this;
}

TimePoint& TimePoint::operator-=(const TimeSpan& timeSpan)
{
	m_timeSpan -= timeSpan;
	return *this;
}



TimeSpan TimePoint::operator-(const TimePoint& rhs) const
{
	return m_timeSpan - rhs.m_timeSpan;
}

bool TimePoint::operator==(const TimePoint & rhs) const
{
	return m_timeSpan == rhs.m_timeSpan;
}

bool TimePoint::operator!=(const TimePoint & rhs) const
{
	return m_timeSpan != rhs.m_timeSpan;
}

bool TimePoint::operator<=(const TimePoint & rhs) const
{
	return m_timeSpan <= rhs.m_timeSpan;
}

bool TimePoint::operator>=(const TimePoint & rhs) const
{
	return m_timeSpan >= rhs.m_timeSpan;
}

bool TimePoint::operator>(const TimePoint & rhs) const
{
	return m_timeSpan > rhs.m_timeSpan;
}

bool TimePoint::operator<(const TimePoint & rhs) const
{
	return m_timeSpan < rhs.m_timeSpan;
}




TimePoint xtest::time::operator+(const TimePoint& t, const TimeSpan& dt)
{
	TimePoint result(t);
	result += dt;
	return result;
}

TimePoint xtest::time::operator+(const TimeSpan& dt, const TimePoint& t)
{
	return t + dt;
}

TimePoint xtest::time::operator-(const TimePoint& t, const TimeSpan& dt)
{
	TimePoint result(t);
	result -= dt;
	return result;
}

TimePoint xtest::time::operator-(const TimeSpan& dt, const TimePoint& t)
{
	return t - dt;
}
