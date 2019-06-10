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
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS; // typeless is required since the shader view and the depth stencil view will interpret it differently
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	XTEST_D3D_CHECK(d3dDevice->CreateTexture2D(&textureDesc, nullptr, &texture));





	// create the view used by the output merger state
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	depthStencilViewDesc.Flags = 0;
	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	XTEST_D3D_CHECK(d3dDevice->CreateDepthStencilView(texture.Get(), &depthStencilViewDesc, &m_depthStencilView));




	//create the view used by the shader
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderViewDesc;
	shaderViewDesc.Format = DXGI_FORMAT_R32_FLOAT; // 24bit red channel (depth), 8 bit unused (stencil)
	shaderViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderViewDesc.Texture2D.MipLevels = 1;
	shaderViewDesc.Texture2D.MostDetailedMip = 0;

	XTEST_D3D_CHECK(d3dDevice->CreateShaderResourceView(texture.Get(), &shaderViewDesc, &m_shaderView));
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

ID3D11DepthStencilView * xtest::render::shading::SSAOMap::AsDepthStencilView()
{
	XTEST_ASSERT(m_depthStencilView, L"shadow map uninitialized");
	return m_depthStencilView.Get();
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
