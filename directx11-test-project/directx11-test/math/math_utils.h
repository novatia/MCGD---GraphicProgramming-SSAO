#pragma once

#include <limits>
#include <cstdint>

namespace xtest {
namespace math {

	bool EqualRelativeEpsilon(float f1, float f2, float epsilon = std::numeric_limits<float>::epsilon());
	bool EqualAbsoluteEpsilon(float f1, float f2, float epsilon = std::numeric_limits<float>::epsilon());
	bool EqualULP(float f1, float f2, unsigned int maxULPDistance = 1);
	bool EqualRelativeAndAbsoluteEpsilon(float f1, float f2, float absoluteEpsilon, float relativeEpsilon = std::numeric_limits<float>::epsilon());
	bool EqualULPAndAbsoluteEpsilon(float f1, float f2, float absoluteEpsilon, unsigned int maxULPDistance = 1);

	unsigned int ULPDistance(float f1, float f2);


	float ToRadians(float degrees);
	float ToDegrees(float radians);

	DirectX::XMFLOAT4 ToFloat4(const DirectX::XMVECTORF32& directXColor);


	template <typename T>
	struct ClampValues
	{
		ClampValues() : minValue(), maxValue() {}
		ClampValues(T min, T max) : minValue(min), maxValue(max) {}
		T minValue;
		T maxValue;
	};

	template <typename T>
	T Clamp(T value, ClampValues<T> clampValues);

	template <typename T>
	T Clamp(T value, T min, T max);

	template <typename T>
	T Lerp(T a, T b, float interpolator);


}// math
}// xtest

#include "math_utils.inl"

