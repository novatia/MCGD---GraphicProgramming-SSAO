#include "stdafx.h"
#include "time_span.h"

using xtest::time::TimeSpan;


namespace {
	int64 ReadHighResolutionTimerFrequency()
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		return int64(frequency.QuadPart);
	}
} // unnamed


float TimeSpan::kHighResTimerFrequency = float(ReadHighResolutionTimerFrequency());


TimeSpan::TimeSpan()
	: m_ticks(0)
{}

TimeSpan::TimeSpan(int64 ticks)
	: m_ticks(ticks)
{}

TimeSpan::TimeSpan(float seconds)
	: m_ticks(int64(seconds * kHighResTimerFrequency))
{}

int64 TimeSpan::Ticks() const
{
	return m_ticks;
}

float TimeSpan::Seconds() const
{
	return float(m_ticks) / kHighResTimerFrequency;
}

float TimeSpan::Millis() const
{
	return Seconds() * 1000.f;
}

void TimeSpan::SetTicks(int64 ticks)
{
	m_ticks = ticks;
}

void TimeSpan::SetSeconds(float seconds)
{
	m_ticks = int64(seconds * kHighResTimerFrequency);
}



TimeSpan& TimeSpan::operator+=(const TimeSpan& rhs)
{
	m_ticks += rhs.m_ticks;
	return *this;
}

TimeSpan& TimeSpan::operator-=(const TimeSpan& rhs)
{
	m_ticks -= rhs.m_ticks;
	return *this;
}

TimeSpan& TimeSpan::operator*=(float scale)
{
	m_ticks = int64((Seconds() * scale) * kHighResTimerFrequency);
	return *this;
}

TimeSpan& TimeSpan::operator*=(int64 scale)
{
	m_ticks *= scale;
	return *this;
}

TimeSpan& TimeSpan::operator*=(uint64 scale)
{
	m_ticks *= scale;
	return *this;
}

TimeSpan& TimeSpan::operator*=(const TimeSpan & rhs)
{
	m_ticks *= rhs.m_ticks;
	return *this;
}

TimeSpan& TimeSpan::operator/=(float divisor)
{
	m_ticks = int64((Seconds() / divisor) * kHighResTimerFrequency);
	return *this;
}

TimeSpan& TimeSpan::operator/=(int64 divisor)
{
	m_ticks /= divisor;
	return *this;
}

TimeSpan& TimeSpan::operator/=(uint64 divisor)
{
	m_ticks /= divisor;
	return *this;
}

TimeSpan& TimeSpan::operator/=(const TimeSpan & rhs)
{
	m_ticks /= rhs.m_ticks;
	return *this;
}




bool TimeSpan::operator==(const TimeSpan & rhs) const
{
	return m_ticks == rhs.m_ticks;
}

bool TimeSpan::operator!=(const TimeSpan& rhs) const
{
	return m_ticks != rhs.m_ticks;
}

bool TimeSpan::operator<=(const TimeSpan & rhs) const
{
	return m_ticks <= rhs.m_ticks;
}

bool TimeSpan::operator>=(const TimeSpan & rhs) const
{
	return m_ticks >= rhs.m_ticks;
}

bool TimeSpan::operator<(const TimeSpan & rhs) const
{
	return m_ticks < rhs.m_ticks;
}

bool TimeSpan::operator>(const TimeSpan & rhs) const
{
	return m_ticks > rhs.m_ticks;
}



TimeSpan xtest::time::operator+(const TimeSpan& lhs, const TimeSpan& rhs)
{
	TimeSpan result(lhs);
	result += rhs;
	return result;
}

TimeSpan xtest::time::operator-(const TimeSpan& lhs, const TimeSpan& rhs)
{
	TimeSpan result(lhs);
	result -= rhs;
	return result;
}

TimeSpan xtest::time::operator*(const TimeSpan& lhs, float scale)
{
	TimeSpan result(lhs);
	result *= scale;
	return result;
}

TimeSpan xtest::time::operator*(float scale, const TimeSpan& rhs)
{
	return rhs * scale;
}

TimeSpan xtest::time::operator*(const TimeSpan& lhs, int64 scale)
{
	TimeSpan result(lhs);
	result *= scale;
	return result;
}

TimeSpan xtest::time::operator*(int64 scale, const TimeSpan& rhs)
{
	return rhs * scale;
}

TimeSpan xtest::time::operator*(const TimeSpan& lhs, uint64 scale)
{
	TimeSpan result(lhs);
	result *= scale;
	return result;
}

TimeSpan xtest::time::operator*(uint64 scale, const TimeSpan& rhs)
{
	return rhs * scale;
}

TimeSpan xtest::time::operator*(const TimeSpan& lhs, const TimeSpan& rhs)
{
	TimeSpan result(lhs);
	result *= rhs;
	return result;
}

TimeSpan xtest::time::operator/(const TimeSpan& lhs, float divisor)
{
	TimeSpan result(lhs);
	result /= divisor;
	return result;
}

TimeSpan xtest::time::operator/(const TimeSpan& lhs, int64 divisor)
{
	TimeSpan result(lhs);
	result /= divisor;
	return result;
}

TimeSpan xtest::time::operator/(const TimeSpan& lhs, uint64 divisor)
{
	TimeSpan result(lhs);
	result /= divisor;
	return result;
}

TimeSpan xtest::time::operator/(const TimeSpan& lhs, const TimeSpan& rhs)
{
	TimeSpan result(lhs);
	result /= rhs;
	return result;
}
