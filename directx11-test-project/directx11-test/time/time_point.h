#pragma once

#include <time\time_span.h>

namespace xtest {
namespace time {

	class TimePoint
	{
	public:

		static TimePoint Now();

		TimePoint();
		explicit TimePoint(const TimeSpan& timeSpan);

		TimePoint& operator+=(const TimeSpan& timeSpan);
		TimePoint& operator-=(const TimeSpan& timeSpan);
		TimeSpan operator-(const TimePoint& rhs) const;

		bool operator==(const TimePoint& rhs) const;
		bool operator!=(const TimePoint& rhs) const;
		bool operator<=(const TimePoint& rhs) const;
		bool operator>=(const TimePoint& rhs) const;
		bool operator>(const TimePoint& rhs) const;
		bool operator<(const TimePoint& rhs) const;

	private:

		TimeSpan m_timeSpan;

	};


	TimePoint operator+(const TimePoint& t, const TimeSpan& dt);
	TimePoint operator+(const TimeSpan& dt, const TimePoint& t);
	TimePoint operator-(const TimePoint& t, const TimeSpan& dt);
	TimePoint operator-(const TimeSpan& dt, const TimePoint& t);


}// time
}// xtest

