#pragma once


namespace xtest {
namespace scene {

	// very basic bounding sphere, a lot of missing functionalities but enough for our purpose
	class BoundingSphere
	{
	public:

		BoundingSphere();
		BoundingSphere(const DirectX::XMFLOAT3& position, float radius);

		void SetPosition(const DirectX::XMFLOAT3& position);
		void SetRadius(float radius);

		DirectX::XMFLOAT3 GetPosition() const;
		float GetRadius() const;


	private:

		DirectX::XMFLOAT3 m_position;
		float m_radius;
	};

} //scene
} //xtest
