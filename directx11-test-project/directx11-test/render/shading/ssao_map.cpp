#include "stdafx.h"
#include "ssao_map.h"
#include <service/locator.h>

xtest::render::shading::SSAOData::VertexInAmbientOcclusion::VertexInAmbientOcclusion(DirectX::XMFLOAT3 inPos, DirectX::XMFLOAT3 inToFarPlaneIndex, DirectX::XMFLOAT2 inUv)
{
	pos = inPos;
	toFarPlaneIndex = inToFarPlaneIndex;
	uv = inUv;
}

bool xtest::render::shading::SSAOData::VertexInAmbientOcclusion::operator==(const VertexInAmbientOcclusion & other) const
{
	return pos.x == other.pos.x
		&& pos.y == other.pos.y
		&& pos.z == other.pos.z
		&& toFarPlaneIndex.x == other.toFarPlaneIndex.x
		&& toFarPlaneIndex.y == other.toFarPlaneIndex.y
		&& toFarPlaneIndex.z == other.toFarPlaneIndex.z
		&& uv.x == other.uv.x
		&& uv.y == other.uv.y;
}

xtest::render::shading::SSAOMap::SSAOMap(uint32 width, uint32 height)
{
	m_width = width;
	m_height = height;

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


	//BIND VERTEX INPUT
	 
	SSAOData::VertexInAmbientOcclusion *v1 = new SSAOData::VertexInAmbientOcclusion(DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 1.0f));
//	SSAOData::VertexInAmbientOcclusion *v2 = new SSAOData::VertexInAmbientOcclusion(DirectX::XMFLOAT3(-1.0f, +1.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f));
//	SSAOData::VertexInAmbientOcclusion *v3 = new SSAOData::VertexInAmbientOcclusion(DirectX::XMFLOAT3(+1.0f, +1.0f, 0.0f), DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f));
//	SSAOData::VertexInAmbientOcclusion *v4 = new SSAOData::VertexInAmbientOcclusion(DirectX::XMFLOAT3(+1.0f, -1.0f, 0.0f), DirectX::XMFLOAT3(3.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));

	SSAOData::VertexInAmbientOcclusion *v2 = new SSAOData::VertexInAmbientOcclusion(DirectX::XMFLOAT3(-1.0f, +1.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 0.0f));
	SSAOData::VertexInAmbientOcclusion *v3 = new SSAOData::VertexInAmbientOcclusion(DirectX::XMFLOAT3(+1.0f, +1.0f, 0.0f), DirectX::XMFLOAT3(2.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(1.0f, 1.0f));
	SSAOData::VertexInAmbientOcclusion *v4 = new SSAOData::VertexInAmbientOcclusion(DirectX::XMFLOAT3(+1.0f, -1.0f, 0.0f), DirectX::XMFLOAT3(3.0f, 0.0f, 0.0f), DirectX::XMFLOAT2(0.0f, 0.0f));

	m_vs_data.vertices.push_back(*v1);
	m_vs_data.vertices.push_back(*v2);
	m_vs_data.vertices.push_back(*v3);
	m_vs_data.vertices.push_back(*v4);

	m_vs_data.indices.push_back(0);
	m_vs_data.indices.push_back(1);
	m_vs_data.indices.push_back(2);
	m_vs_data.indices.push_back(0);
	m_vs_data.indices.push_back(2);
	m_vs_data.indices.push_back(3);
	
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = UINT(sizeof(SSAOData::VertexInAmbientOcclusion) * m_vs_data.vertices.size());
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexInitData;
	vertexInitData.pSysMem = &m_vs_data.vertices[0];
	XTEST_D3D_CHECK(service::Locator::GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexInitData, &m_d3dVertexBuffer));

	// index buffer
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexBufferDesc.ByteWidth = UINT(sizeof(uint32) * m_vs_data.indices.size());
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexInitdata;
	indexInitdata.pSysMem = &m_vs_data.indices[0];
	XTEST_D3D_CHECK(service::Locator::GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexInitdata, &m_d3dIndexBuffer));

	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;
	m_viewport.Width = m_width;
	m_viewport.Height = m_height;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
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

void xtest::render::shading::SSAOMap::Bind()
{
	XTEST_ASSERT(m_d3dVertexBuffer && m_d3dIndexBuffer, L"uninitialized renderable");

	UINT stride = sizeof(SSAOData::VertexInAmbientOcclusion);
	UINT offset = 0;
	service::Locator::GetD3DContext()->IASetVertexBuffers(0, 1, m_d3dVertexBuffer.GetAddressOf(), &stride, &offset);
	service::Locator::GetD3DContext()->IASetIndexBuffer(m_d3dIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}

void xtest::render::shading::SSAOMap::Draw()
{
	Bind();
	service::Locator::GetD3DContext()->DrawIndexed(m_vs_data.indices.size(), 0, 0);
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
