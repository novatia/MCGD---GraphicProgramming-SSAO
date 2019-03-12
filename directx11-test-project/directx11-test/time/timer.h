#pragma once

#include <functional>
#include <time/time_span.h>
#include <time/time_point.h>

namespace xtest {
namespace time {

	class TimePoint;
	class TimeSpan;

	class Timer
	{
	public:

		explicit Timer(const TimePoint& time = TimePoint::Now());

		// the scale can be negative, in that case the timer will run backwards
		void SetTimeScale(float scale);
		float GetTimeScale() const;

		TimeSpan ElapsedTime() const;
		const TimePoint& CurrentTime() const;
		const TimePoint& StartingTime() const;
		const TimeSpan& DeltaTime() const;
		TimeSpan PauseDuration() const;
		const TimePoint& PauseStartTime() const;

		// Updates the timer's time, has no effects when the clock is paused.
		void Update();
		void Update(const TimeSpan& dt);
		void UpdateIfPaused(const TimeSpan& dt);

		void SetPause(bool wantPause);
		bool IsPaused() const;

		// Sets a callback function to be invoked after the given time delay is passed
		void SetAlarm(const TimeSpan& timeDelay, std::function<void(void)> alarmCallback);
		void ClearAlarm();
		bool HasAlarmSet() const;

		void Reset(const TimePoint& time = TimePoint::Now());


	private:

		TimePoint m_startingTime;
		TimePoint m_time;
		TimePoint m_pauseStart;
		TimeSpan m_alarmTimeDelay;
		TimeSpan m_deltaTime;
		float m_scale;
		bool m_paused;
		bool m_hasAlarmSet;
		std::function<void(void)> m_alarmCallback;

	};


} // time
} // xtest

