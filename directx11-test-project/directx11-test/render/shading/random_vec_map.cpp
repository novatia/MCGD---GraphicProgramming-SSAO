#include "stdafx.h"
#include <service/locator.h>
#include "random_vec_map.h"
#include <DirectXPackedVector.h>


using namespace xtest::render::shading;
using namespace DirectX;

RandomVectorMap::RandomVectorMap(uint32 width, uint32 height)
	: m_width(width)
	, m_height(height)
	, m_shaderView(nullptr)

{
	m_viewport.TopLeftX = 0.f;
	m_viewport.TopLeftY = 0.f;
	m_viewport.Width = static_cast<float>(m_width);
	m_viewport.Height = static_cast<float>(m_height);
	m_viewport.MinDepth = 0.f;
	m_viewport.MaxDepth = 1.f;
}

float RandomFloat1()
{
	return (float)(rand()) / (float)RAND_MAX;
}

void RandomVectorMap::Init()
{
	// already initialized
	if (m_shaderView)
	{
		return;
	}

	ID3D11Device* d3dDevice = service::Locator::GetD3DDevice();
	// create the shadow map texture
	D3D11_TEXTURE2D_DESC textureDesc;

	textureDesc.Width = 256;
	textureDesc.Height = 256;

	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // modify
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.SysMemPitch = 256 * sizeof(DirectX::PackedVector::XMCOLOR);

	DirectX::PackedVector::XMCOLOR color[256 * 256];
	for (int i = 0; i < 256; ++i)
	{
		for (int j = 0; j < 256; ++j)
		{
			XMFLOAT3 v(RandomFloat1(), RandomFloat1(), RandomFloat1());
			color[i * 256 + j] = DirectX::PackedVector::XMCOLOR(v.x, v.y, v.z, 0.0f);
		}
	}
	initData.pSysMem = color;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
	XTEST_ASSERT(d3dDevice->CreateTexture2D(&textureDesc, &initData, &texture));

	XTEST_ASSERT(d3dDevice->CreateShaderResourceView(texture.Get(), 0, &m_shaderView));
}


ID3D11ShaderResourceView* RandomVectorMap::AsShaderView()
{
	XTEST_ASSERT(m_shaderView, L"random vec map uninitialized");
	return m_shaderView.Get();
}



D3D11_VIEWPORT RandomVectorMap::Viewport() const
{
	return m_viewport;
}




uint32 RandomVectorMap::Width() const
{
	return m_width;
}

uint32 RandomVectorMap::Height() const
{
	return m_height;
}
