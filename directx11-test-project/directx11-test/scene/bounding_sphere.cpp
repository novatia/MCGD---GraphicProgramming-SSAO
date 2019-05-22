#include "stdafx.h"
#include "bounding_sphere.h"

using namespace xtest::scene;


BoundingSphere::BoundingSphere()
	: m_position(0.f, 0.f, 0.f)
	, m_radius(0.f)
{}


BoundingSphere::BoundingSphere(const DirectX::XMFLOAT3& position, float radius)
	: m_position(position)
	, m_radius(radius)
{}


void BoundingSphere::SetPosition(const DirectX::XMFLOAT3& position)
{
	m_position = position;
}


void xtest::scene::BoundingSphere::SetRadius(float radius)
{
	m_radius = radius;
}


DirectX::XMFLOAT3 BoundingSphere::GetPosition() const
{
	return m_position;
}


float BoundingSphere::GetRadius() const
{
	return m_radius;
}
