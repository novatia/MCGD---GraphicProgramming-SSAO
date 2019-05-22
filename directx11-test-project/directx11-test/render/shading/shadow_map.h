#pragma once

#include <render/render_resource.h>
#include <scene/bounding_sphere.h>


namespace xtest {
namespace render {
namespace shading {

	// class representing a shadow map
	class ShadowMap
	{
	public:

		explicit ShadowMap(uint32 resolution);

		void Init();

		void SetTargetBoundingSphere(const scene::BoundingSphere& boundingSphere);
		void SetLight(const DirectX::XMFLOAT3& dirLight, const DirectX::XMFLOAT3& up = { 0.f,1.f,0.f });

		ID3D11ShaderResourceView* AsShaderView();
		ID3D11DepthStencilView* AsDepthStencilView();
		D3D11_VIEWPORT Viewport() const;
		uint32 Resolution() const;
		DirectX::XMMATRIX LightViewMatrix();
		DirectX::XMMATRIX LightProjMatrix();
		DirectX::XMMATRIX VPTMatrix();
		
		
	private:

		void CalcMatrices();


		uint32 m_resolution;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderView;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
		D3D11_VIEWPORT m_viewport;
		scene::BoundingSphere m_bSphere;
		DirectX::XMFLOAT3 m_lightDir;
		DirectX::XMFLOAT3 m_up;
		DirectX::XMFLOAT4X4 m_V;
		DirectX::XMFLOAT4X4 m_P;
		DirectX::XMFLOAT4X4 m_VPT;
		bool m_isDirty;
	};

} //shading
} //render
} //xtest
