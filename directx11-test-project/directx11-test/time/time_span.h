#pragma once


namespace xtest {
namespace time {

	class TimeSpan
	{
	public:

		TimeSpan();
		explicit TimeSpan(int64 ticks);
		explicit TimeSpan(float seconds);

		int64 Ticks() const;
		float Seconds() const;
		float Millis() const;

		void SetTicks(int64 ticks);
		void SetSeconds(float seconds);

		TimeSpan& operator+=(const TimeSpan& rhs);
		TimeSpan& operator-=(const TimeSpan& rhs);
		TimeSpan& operator*=(float scale);
		TimeSpan& operator*=(int64 scale);
		TimeSpan& operator*=(uint64 scale);
		TimeSpan& operator*=(const TimeSpan& rhs);
		TimeSpan& operator/=(float divisor);
		TimeSpan& operator/=(int64 divisor);
		TimeSpan& operator/=(uint64 divisor);
		TimeSpan& operator/=(const TimeSpan& rhs);


		bool operator==(const TimeSpan& rhs) const;
		bool operator!=(const TimeSpan& rhs) const;
		bool operator<=(const TimeSpan& rhs) const;
		bool operator>=(const TimeSpan& rhs) const;
		bool operator<(const TimeSpan& rhs) const;
		bool operator>(const TimeSpan& rhs) const;


	private:

		static float kHighResTimerFrequency;

		int64 m_ticks;
	};


	TimeSpan operator+(const TimeSpan& lhs, const TimeSpan& rhs);
	TimeSpan operator-(const TimeSpan& lhs, const TimeSpan& rhs);
	TimeSpan operator*(const TimeSpan& lhs, float scale);
	TimeSpan operator*(float scale, const TimeSpan& rhs);
	TimeSpan operator*(const TimeSpan& lhs, int64 scale);
	TimeSpan operator*(int64 scale, const TimeSpan& rhs);
	TimeSpan operator*(const TimeSpan& lhs, uint64 scale);
	TimeSpan operator*(uint64 scale, const TimeSpan& rhs);
	TimeSpan operator*(const TimeSpan& lhs, const TimeSpan& rhs);
	TimeSpan operator/(const TimeSpan& lhs, float divisor);
	TimeSpan operator/(const TimeSpan& lhs, int64 divisor);
	TimeSpan operator/(const TimeSpan& lhs, uint64 divisor);
	TimeSpan operator/(const TimeSpan& lhs, const TimeSpan& rhs);

}// time
}// xtest

