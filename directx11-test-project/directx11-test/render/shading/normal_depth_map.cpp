#include "stdafx.h"
#include "shadow_map.h"
#include <service/locator.h>
#include "normal_depth_map.h"


using namespace xtest::render::shading;
using namespace DirectX;

NormalDepthMap::NormalDepthMap(uint32 width, uint32 height)
	: m_width(width)
	, m_height(height)
	, m_shaderView(nullptr)
	, m_renderTargetView(nullptr)
	, m_depthStencilView(nullptr)
{
	m_viewport.TopLeftX = 0.f;
	m_viewport.TopLeftY = 0.f;
	m_viewport.Width = static_cast<float>(m_width);
	m_viewport.Height = static_cast<float>(m_height);
	m_viewport.MinDepth = 0.f;
	m_viewport.MaxDepth = 1.f;
}


void NormalDepthMap::Init()
{
	// already initialized
	if (m_shaderView)
	{
		return;
	}

	ID3D11Device* d3dDevice = service::Locator::GetD3DDevice();
	// create the shadow map texture
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = m_width;
	textureDesc.Height = m_height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // modify
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = (D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	XTEST_D3D_CHECK(d3dDevice->CreateTexture2D(&textureDesc, nullptr, &texture));

	// create the view used by the output merger state
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	XTEST_D3D_CHECK(d3dDevice->CreateRenderTargetView(texture.Get(), 0, &m_renderTargetView));

	//create the view used by the shader
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderViewDesc;
	shaderViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	shaderViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderViewDesc.Texture2D.MipLevels = 1;
	shaderViewDesc.Texture2D.MostDetailedMip = 0;

	XTEST_D3D_CHECK(d3dDevice->CreateShaderResourceView(texture.Get(), &shaderViewDesc, &m_shaderView));
}


ID3D11ShaderResourceView* NormalDepthMap::AsShaderView()
{
	XTEST_ASSERT(m_shaderView, L"shadow map uninitialized");
	return m_shaderView.Get();
}


ID3D11RenderTargetView** NormalDepthMap::AsRenderTargetView()
{
	XTEST_ASSERT(m_renderTargetView, L"shadow map uninitialized");
	return m_renderTargetView.GetAddressOf();
}

ID3D11RenderTargetView* NormalDepthMap::AsRenderTargetView1()
{
	XTEST_ASSERT(m_renderTargetView, L"shadow map uninitialized");
	return m_renderTargetView.Get();
}

ID3D11DepthStencilView* NormalDepthMap::AsDepthStencilView()
{
	XTEST_ASSERT(m_depthStencilView, L"shadow map uninitialized");
	return m_depthStencilView.Get();
}

D3D11_VIEWPORT NormalDepthMap::Viewport() const
{
	return m_viewport;
}




uint32 NormalDepthMap::Width() const
{
	return m_width;
}

uint32 NormalDepthMap::Height() const
{
	return m_height;
}
