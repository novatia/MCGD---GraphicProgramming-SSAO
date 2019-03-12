#include "stdafx.h"
#include "spherical_camera.h"


using namespace xtest;
using namespace xtest::camera;

SphericalCamera::SphericalCamera(
	float polarAngleRadians, 
	float azimuthAngleRadians, 
	float radius, 
	const DirectX::XMFLOAT3& pivot,
	const DirectX::XMFLOAT3& alignUpDir /*= DirectX::XMFLOAT3(0.f, 1.f, 0.f)*/,
	const math::ClampValues<float>& polarAngleLimits /*= math::ClampValues<float>(math::ToRadians(1.f), math::ToRadians(179.f))*/,
	const math::ClampValues<float>& radiusLimits /*= math::ClampValues<float>(1.f, 100.f) */)
{
	XTEST_ASSERT(polarAngleRadians > 0.f);
	XTEST_ASSERT(azimuthAngleRadians > 0.f);
	XTEST_ASSERT(radius > 0.f);

	m_polarAngle = polarAngleRadians;
	m_azimuthAngle = azimuthAngleRadians;
	m_radius = radius;
	m_polarAngleLimits = polarAngleLimits;
	m_radiusLimits = radiusLimits;

	m_pivot = pivot;
	m_upDir = alignUpDir;

}


void SphericalCamera::RotateBy(float deltaPolarAngle, float deltaAzimuthAngle)
{
	m_polarAngle = math::Clamp(m_polarAngle + deltaPolarAngle, m_polarAngleLimits);
	m_azimuthAngle = std::fmod(m_azimuthAngle + deltaAzimuthAngle, DirectX::XM_2PI);
}

void SphericalCamera::IncreaseRadiusBy(float deltaRadiusLength)
{
	m_radius = math::Clamp(m_radius + deltaRadiusLength, m_radiusLimits);
}

DirectX::XMFLOAT3 SphericalCamera::GetPosition() const
{
	return DirectX::XMFLOAT3(
		std::sinf(m_polarAngle) * std::sinf(m_azimuthAngle) * m_radius,
		std::cosf(m_polarAngle) * m_radius,
		std::sinf(m_polarAngle) * std::cos(m_azimuthAngle) * m_radius
	);
}

DirectX::XMMATRIX SphericalCamera::GetViewMatrix() const
{
	DirectX::XMFLOAT3 position = GetPosition();

	return DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorSet(position.x, position.y, position.z, 1.f), 
		DirectX::XMVectorSet(m_pivot.x, m_pivot.y, m_pivot.z, 1.f), 
		DirectX::XMVectorSet(m_upDir.x, m_upDir.y, m_upDir.z, 0.f)
	);
}

float SphericalCamera::GetPolarAngle() const
{
	return m_polarAngle;
}

float SphericalCamera::GetAzimuthAngle() const
{
	return m_azimuthAngle;
}

float SphericalCamera::GetRadius() const
{
	return m_radius;
}

void SphericalCamera::SetRotation(float polarAngle, float azimuthAngle)
{
	m_polarAngle = math::Clamp(polarAngle, m_polarAngleLimits);
	m_azimuthAngle = std::fmod(azimuthAngle, DirectX::XM_2PI);
}

void SphericalCamera::SetRadius(float radiusLength)
{
	m_radius = math::Clamp(radiusLength, m_radiusLimits);
}
