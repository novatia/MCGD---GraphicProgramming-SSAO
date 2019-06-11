#include "stdafx.h"
#include <service/locator.h>
#include "random_vec_map.h"

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

float RandomFloat1(float min, float max)
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
	int SIZE = 256;
	ID3D11Device* d3dDevice = service::Locator::GetD3DDevice();
	// create the shadow map texture
	D3D11_TEXTURE2D_DESC textureDesc;

	textureDesc.Width = SIZE;
	textureDesc.Height = SIZE;
	int bpp = 12;
	int nb_color = 3;
	float  *color = new float[SIZE * SIZE * nb_color];

	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage		= D3D11_USAGE_DEFAULT;
	textureDesc.Format		= DXGI_FORMAT_R32G32B32_FLOAT; // modify
	textureDesc.BindFlags	= D3D11_BIND_SHADER_RESOURCE;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	for (int i = 0; i < SIZE; i+=3)
	{
		for (int j = 0; j < SIZE; j+=3)
		{
			 color[i * SIZE + j +0] = RandomFloat1(1, 255);
			 color[i * SIZE + j +1] = RandomFloat1(1, 255);
			 color[i * SIZE + j +2] = RandomFloat1(1, 255);
		}
	}

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem			= (void *)color;
	initData.SysMemPitch		= SIZE * bpp;
	initData.SysMemSlicePitch	= SIZE * SIZE * bpp;

 	Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
	HRESULT hr = d3dDevice->CreateTexture2D(&textureDesc, &initData, tex.GetAddressOf());
	//S_OK

	delete[] color;
	HRESULT hrsh = d3dDevice->CreateShaderResourceView( tex.Get(), 0, &m_shaderView);
	//S_OK
}


ID3D11ShaderResourceView* RandomVectorMap::AsShaderView()
{
	XTEST_ASSERT(m_shaderView, L"shadow map uninitialized");
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
