#include "stdafx.h"
#include "shadow_map.h"
#include <service/locator.h>


using namespace xtest::render::shading;
using namespace DirectX;

ShadowMap::ShadowMap(uint32 resolution)
	: m_resolution(resolution)
	, m_shaderView(nullptr)
	, m_depthStencilView(nullptr)
	, m_bSphere()
	, m_lightDir()
	, m_up()
	, m_V()
	, m_P()
	, m_VPT()
	, m_isDirty(true)
{
	m_viewport.TopLeftX = 0.f;
	m_viewport.TopLeftY = 0.f;
	m_viewport.Width = static_cast<float>(m_resolution);
	m_viewport.Height = static_cast<float>(m_resolution);
	m_viewport.MinDepth = 0.f;
	m_viewport.MaxDepth = 1.f;
}


void ShadowMap::Init()
{
	// already initialized
	if (m_shaderView)
	{
		return;
	}

	ID3D11Device* d3dDevice = service::Locator::GetD3DDevice();


	// create the shadow map texture
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = m_resolution;
	textureDesc.Height = m_resolution;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; // typeless is required since the shader view and the depth stencil view will interpret it differently
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
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	XTEST_D3D_CHECK(d3dDevice->CreateDepthStencilView(texture.Get(), &depthStencilViewDesc, &m_depthStencilView));



	//create the view used by the shader
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderViewDesc;
	shaderViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; // 24bit red channel (depth), 8 bit unused (stencil)
	shaderViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderViewDesc.Texture2D.MipLevels = 1;
	shaderViewDesc.Texture2D.MostDetailedMip = 0;

	XTEST_D3D_CHECK(d3dDevice->CreateShaderResourceView(texture.Get(), &shaderViewDesc, &m_shaderView));
}


void ShadowMap::SetTargetBoundingSphere(const xtest::scene::BoundingSphere& boundingSphere)
{
	XTEST_ASSERT(boundingSphere.GetRadius() > 0.f);
	m_bSphere = boundingSphere;
	m_isDirty = true;
}


void ShadowMap::SetLight(const DirectX::XMFLOAT3& dirLight, const DirectX::XMFLOAT3& up)
{
	m_lightDir = dirLight;
	m_up = up;
	XMStoreFloat3(&m_lightDir, XMVector3Normalize(XMLoadFloat3(&m_lightDir)));
	XMStoreFloat3(&m_up, XMVector3Normalize(XMLoadFloat3(&m_up)));
	m_isDirty = true;
}


ID3D11ShaderResourceView* ShadowMap::AsShaderView()
{
	XTEST_ASSERT(m_shaderView, L"shadow map uninitialized");
	return m_shaderView.Get();
}


ID3D11DepthStencilView* ShadowMap::AsDepthStencilView()
{
	XTEST_ASSERT(m_depthStencilView, L"shadow map uninitialized");
	return m_depthStencilView.Get();
}


D3D11_VIEWPORT ShadowMap::Viewport() const
{
	return m_viewport;
}


void ShadowMap::CalcMatrices()
{
	XTEST_ASSERT(m_bSphere.GetRadius() > 0.f, L"no bounding sphere was set");
	XTEST_ASSERT(XMVectorGetX(XMVector3Length(XMLoadFloat3(&m_lightDir))) > 0.f, L"no light direction was set");

	XMFLOAT3 bSpherePosW = m_bSphere.GetPosition();

	// place the "camera light" to the edge of the sphere box
	XMVECTOR bSpherePos = XMLoadFloat3(&bSpherePosW);
	XMVECTOR lightDir = XMLoadFloat3(&m_lightDir);
	XMVECTOR lightPos = bSpherePos + (-m_bSphere.GetRadius()*lightDir);

	// light view matrix
	XMMATRIX V = XMMatrixLookAtLH(lightPos, bSpherePos, XMLoadFloat3(&m_up));
	XMStoreFloat4x4(&m_V, V);



	XMFLOAT3 bSpherePosLS;
	XMStoreFloat3(&bSpherePosLS, XMVector3Transform(bSpherePos, V));

	// light projection matrix
	XMMATRIX P = XMMatrixOrthographicOffCenterLH(
		bSpherePosLS.x - m_bSphere.GetRadius(),
		bSpherePosLS.x + m_bSphere.GetRadius(),
		bSpherePosLS.y - m_bSphere.GetRadius(),
		bSpherePosLS.y + m_bSphere.GetRadius(),
		bSpherePosLS.z - m_bSphere.GetRadius(),
		bSpherePosLS.z + m_bSphere.GetRadius()
	);

	XMStoreFloat4x4(&m_P, P);
	


	// used to map NDC coordinates to the texture space [-1, 1] -> [0,1]
	XMMATRIX T{
		0.5f,  0.f , 0.f, 0.f,
		0.0f, -0.5f, 0.f, 0.f,
		0.0f,  0.f , 1.f, 0.f,
		0.5f,  0.5f, 0.f, 1.f,
	};

	XMMATRIX VPT = V*P*T;
	XMStoreFloat4x4(&m_VPT, VPT);
	m_isDirty = false;
}


XMMATRIX ShadowMap::LightViewMatrix()
{
	if (m_isDirty)
	{
		CalcMatrices();
	}
	return XMLoadFloat4x4(&m_V);
}


XMMATRIX ShadowMap::LightProjMatrix()
{
	if (m_isDirty)
	{
		CalcMatrices();
	}
	return XMLoadFloat4x4(&m_P);
}


XMMATRIX ShadowMap::VPTMatrix()
{
	if (m_isDirty)
	{
		CalcMatrices();
	}
	return XMLoadFloat4x4(&m_VPT);
}

uint32 ShadowMap::Resolution() const
{
	return m_resolution;
}

