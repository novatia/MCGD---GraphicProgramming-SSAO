#include "math_utils.h"
#pragma once


template<typename T>
T xtest::math::Clamp(T value, ClampValues<T> clampValues)
{
	return Clamp(value, clampValues.minValue, clampValues.maxValue);
}

template<typename T>
T xtest::math::Clamp(T value, T min, T max)
{
	return std::max(min, std::min(value, max));
}

template <typename T>
T xtest::math::Lerp(T a, T b, float interpolator)
{
	return T(a * (1.f - interpolator) + b * (interpolator));
}


