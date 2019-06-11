#include "stdafx.h"
#include "ssao_map.h"
#include <service/locator.h>

xtest::render::shading::SSAOMap::SSAOMap(uint32 width, uint32 height)
{
	m_width = width / 2;
	m_height = height /2;

	//m_kernel_size = kernel_size;
	//m_noise_size = noise_size;
	//m_radius = radius;
	//m_power = power;
}

void xtest::render::shading::SSAOMap::Init()
{
	// already initialized
	if (m_shaderView)
	{
		return;
	}

	ID3D11Device* d3dDevice = service::Locator::GetD3DDevice();
	// create the ssao map texture
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = m_width;
	textureDesc.Height = m_height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R16_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	XTEST_D3D_CHECK(d3dDevice->CreateTexture2D(&textureDesc, nullptr, &texture));

	XTEST_D3D_CHECK(d3dDevice->CreateShaderResourceView(texture.Get(), 0, &m_shaderView));

	XTEST_D3D_CHECK(d3dDevice->CreateRenderTargetView(texture.Get(), 0, &m_renderTargetView));
}

//void xtest::render::shading::SSAOMap::SetNoiseSize(uint32 noise_size)
//{
//	m_noise_size = noise_size;
//}
//
//void xtest::render::shading::SSAOMap::SetKernelSize(uint32 kernel_size)
//{
//	m_kernel_size = kernel_size;
//}
//
//void xtest::render::shading::SSAOMap::SetRadius(float radius)
//{
//	m_radius = radius;
//}
//
//void xtest::render::shading::SSAOMap::SetPower(float power)
//{
//	m_power = power;
//}

ID3D11ShaderResourceView * xtest::render::shading::SSAOMap::AsShaderView()
{
	XTEST_ASSERT(m_shaderView, L"shadow map uninitialized");
	return m_shaderView.Get();
}

ID3D11RenderTargetView * xtest::render::shading::SSAOMap::AsRenderTargetView()
{
	XTEST_ASSERT(m_renderTargetView, L"shadow map uninitialized");
	return m_renderTargetView.Get();
}

D3D11_VIEWPORT xtest::render::shading::SSAOMap::Viewport() const
{
	return m_viewport;
}

uint32 xtest::render::shading::SSAOMap::Width() const
{
	return m_width;
}
uint32 xtest::render::shading::SSAOMap::Height() const
{
	return m_height;
}

//uint32 xtest::render::shading::SSAOMap::NoiseSize() const
//{
//	return m_noise_size;
//}
//
//uint32 xtest::render::shading::SSAOMap::KernelSize() const
//{
//	return m_kernel_size;
//}
//
//float xtest::render::shading::SSAOMap::Radius() const
//{
//	return m_radius;
//}
//
//float xtest::render::shading::SSAOMap::Power() const
//{
//	return m_power;
//}
