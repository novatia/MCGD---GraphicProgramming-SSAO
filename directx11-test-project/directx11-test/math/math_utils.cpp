#include "stdafx.h"
#include "math_utils.h"


bool xtest::math::EqualRelativeEpsilon(float f1, float f2, float epsilon)
{
	return std::fabs(f1 - f2) <= std::fmax(f1, f2) * epsilon;
}

bool xtest::math::EqualAbsoluteEpsilon(float f1, float f2, float epsilon)
{
	return std::fabs(f1 - f2) <= epsilon;
}

bool xtest::math::EqualULP(float f1, float f2, unsigned int maxULPDistance)
{
	if (std::signbit(f1) != std::signbit(f2))
	{
		return f1 == f2; //+0.0, -0.0 case
	}
	else
	{
		return ULPDistance(f1, f2) <= maxULPDistance;
	}
}

bool xtest::math::EqualRelativeAndAbsoluteEpsilon(float f1, float f2, float absoluteEpsilon, float relativeEpsilon)
{
	return EqualAbsoluteEpsilon(f1, f2, absoluteEpsilon) || EqualRelativeEpsilon(f1, f2, relativeEpsilon);
}

bool xtest::math::EqualULPAndAbsoluteEpsilon(float f1, float f2, float absoluteEpsilon, unsigned int maxULPDistance)
{
	return EqualAbsoluteEpsilon(f1, f2, absoluteEpsilon) || EqualULP(f1, f2, maxULPDistance);
}

unsigned int xtest::math::ULPDistance(float f1, float f2)
{
	XTEST_STATIC_ASSERT(sizeof(float) <= sizeof(uint32), "ULPDistance probably unsafe");

	union ULP
	{
		float as_float;
		int32 as_int32;
	};

	ULP ulp1;
	ULP ulp2;
	ulp1.as_float = f1;
	ulp2.as_float = f2;

	return std::abs(ulp1.as_int32 - ulp2.as_int32);
}

float xtest::math::ToRadians(float degrees)
{
	return degrees * (DirectX::XM_PI / 180.f);
}

float xtest::math::ToDegrees(float radians)
{
	return radians * (180.f / DirectX::XM_PI);
}

DirectX::XMFLOAT4 xtest::math::ToFloat4(const DirectX::XMVECTORF32& directXColor)
{
	return DirectX::XMFLOAT4(directXColor.f[0], directXColor.f[1], directXColor.f[2], directXColor.f[3]);
}
