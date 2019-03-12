#include "stdafx.h"
#include "timer.h"


using xtest::time::Timer;
using xtest::time::TimeSpan;
using xtest::time::TimePoint;

static const TimeSpan kTimeSpanZero = TimeSpan();


Timer::Timer(const TimePoint& time/*= TimePoint::Now()*/)
	: m_startingTime(time)
	, m_time(time)
	, m_pauseStart()
	, m_alarmTimeDelay()
	, m_deltaTime()
	, m_scale(1.f)
	, m_paused(false)
	, m_hasAlarmSet(false)
	, m_alarmCallback()
{}

void Timer::SetTimeScale(float scale)
{
	m_scale = scale;
}

float Timer::GetTimeScale() const
{
	return m_scale;
}

TimeSpan Timer::ElapsedTime() const
{
	return m_time - m_startingTime;
}

const TimePoint& Timer::CurrentTime() const
{
	return m_time;
}

const TimePoint& Timer::StartingTime() const
{
	return m_startingTime;
}

const TimeSpan& Timer::DeltaTime() const
{
	return m_deltaTime;
}

TimeSpan Timer::PauseDuration() const
{
	return TimePoint::Now() - m_pauseStart;
}

const TimePoint& Timer::PauseStartTime() const
{
	return m_pauseStart;
}

void Timer::Update()
{
	Update(TimePoint::Now() - CurrentTime());
}

void Timer::Update(const TimeSpan& dt)
{
	if (!m_paused)
	{
		m_deltaTime = m_scale * dt;
		m_time += m_deltaTime;

		if (m_hasAlarmSet && m_alarmCallback)
		{
			m_alarmTimeDelay -= TimeSpan(std::abs(m_deltaTime.Ticks()));
			if (m_alarmTimeDelay <= kTimeSpanZero)
			{
				m_alarmCallback();
				ClearAlarm();
			}
		}
	}
}

void Timer::UpdateIfPaused(const TimeSpan& dt)
{
	if (m_paused)
	{
		m_deltaTime = m_scale * dt;
		m_time += m_deltaTime;
	}
}

void Timer::SetPause(bool wantPause)
{
	m_paused = wantPause;
	if (wantPause)
	{
		m_pauseStart = TimePoint::Now();
	}
	else
	{
		// when exit the pause we register the current time so the
		// next delta time won't include the paused time
		m_time = TimePoint::Now();
	}
}

bool Timer::IsPaused() const
{
	return m_paused;
}

void Timer::SetAlarm(const TimeSpan& timeDelay, std::function<void(void)> alarmCallback)
{
	m_alarmTimeDelay = TimeSpan(std::abs(timeDelay.Ticks()));
	m_alarmCallback = alarmCallback;
	m_hasAlarmSet = true;
}

void Timer::ClearAlarm()
{
	m_hasAlarmSet = false;
	m_alarmTimeDelay = TimeSpan();
	m_alarmCallback = nullptr;
}

bool Timer::HasAlarmSet() const
{
	return m_hasAlarmSet;
}

void Timer::Reset(const TimePoint& time)
{
	m_startingTime = m_time = time;
	m_deltaTime = TimeSpan();
	ClearAlarm();
}

