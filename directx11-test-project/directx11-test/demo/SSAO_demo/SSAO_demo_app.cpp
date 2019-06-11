#include "stdafx.h"
#include "SSAO_demo_app.h"
#include <file/file_utils.h>
#include <math/math_utils.h>
#include <service/locator.h>
#include <render/shading/vertex_input_types.h>
#include <render/shading/rasterizer_state_types.h>
#include <render/shading/sampler_types.h>
#include <external_libs/directxtk/WICTextureLoader.h>
#include <stdlib.h>
#include <ctime>

using namespace DirectX;
using namespace xtest;
using namespace xtest::render::shading;
using namespace xtest::demo;
using Microsoft::WRL::ComPtr;


SSAODemoApp::SSAODemoApp(HINSTANCE instance,
	const application::WindowSettings& windowSettings,
	const application::DirectxSettings& directxSettings,
	uint32 fps /*=60*/)
	: application::DirectxApp(instance, windowSettings, directxSettings, fps)
	, m_dirKeyLight()
	, m_dirFillLight()
	, m_rarelyChangedData()
	, m_isRarelyChangedDataDirty(true)
	, m_camera(math::ToRadians(80.f), math::ToRadians(170.f), 15.f, { 5.f, 4.f, -5.f }, { 0.f, 1.f, 0.f }, { math::ToRadians(4.f), math::ToRadians(175.f) }, { 3.f, 50.f })
	, m_objects()
	, m_normalDepthPass()
	, m_shadowPass()
	, m_SSAOPass()
	, m_renderPass()
	, m_shadowMap(2048)
	, m_SSAOMap(windowSettings.width, windowSettings.height)//, 1, 128, 4, 2.0f, 3.0f)
	, m_normalDepthMap(windowSettings.width, windowSettings.height)
	, m_randomVecMap(256, 256)
	, m_sceneBoundingSphere({ 0.f, 0.f, 0.f }, 21.f)
{}


SSAODemoApp::~SSAODemoApp()
{}


void SSAODemoApp::Init()
{
	application::DirectxApp::Init();

	m_camera.SetPerspectiveProjection(math::ToRadians(45.f), AspectRatio(), 0.2f, 100.f);

	InitLights();
	InitRenderTechnique();
	InitRenderables();
	service::Locator::GetMouse()->AddListener(this);
	service::Locator::GetKeyboard()->AddListener(this, { input::Key::F, input::Key::F1, input::Key::F2, input::Key::U, input::Key::Y, input::Key::J, input::Key::H, input::Key::M, input::Key::N });
}


void SSAODemoApp::InitLights()
{
	m_dirKeyLight.ambient = { 0.16f, 0.18f, 0.18f, 1.f };
	m_dirKeyLight.diffuse = { 0.8f ,  0.8f,  0.7f, 1.f };
	m_dirKeyLight.specular = { 0.8f ,  0.8f,  0.7f, 1.f };
	XMStoreFloat3(&m_dirKeyLight.dirW, XMVector3Normalize(-XMVectorSet(0.917053342f, 0.390566736f, 0.0802310705f, 0.f)));

	m_dirFillLight.ambient = { 0.01f * 0.16f , 0.01f * 0.18f, 0.005f * 0.18f, 1.f };
	m_dirFillLight.diffuse = { 0.02f * 1.f   , 0.01f * 0.91f, 0.005f * 0.85f, 1.f };
	m_dirFillLight.specular = { 0.01f * 0.087f, 0.01f * 0.09f, 0.005f * 0.094f, 1.f };
	XMStoreFloat3(&m_dirFillLight.dirW, XMVector3Transform(XMLoadFloat3(&m_dirKeyLight.dirW), XMMatrixRotationY(math::ToRadians(45.f))));

	m_rarelyChangedData.useShadowMap = true;
	m_rarelyChangedData.useSSAOMap = true;
}


void SSAODemoApp::InitRenderTechnique()
{
	file::ResourceLoader* loader = service::Locator::GetResourceLoader();


	// shadow pass
	{
		m_shadowMap.SetTargetBoundingSphere(m_sceneBoundingSphere);
		m_shadowMap.SetLight(m_dirKeyLight.dirW);
		m_shadowMap.Init();

		m_rarelyChangedData.shadowMapResolution = float(m_shadowMap.Resolution());

		std::shared_ptr<VertexShader> vertexShader = std::make_shared<VertexShader>(loader->LoadBinaryFile(GetRootDir().append(L"\\shadow_demo_shadowmap_VS.cso")));
		vertexShader->SetVertexInput(std::make_shared<PosOnlyVertexInput>());
		vertexShader->AddConstantBuffer(CBufferFrequency::per_object, std::make_unique<CBuffer<PerObjectShadowMapData>>());

		m_shadowPass.SetState(std::make_shared<RenderPassState>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_shadowMap.Viewport(), std::make_shared<SolidCullBackDepthBiasRS>(), nullptr, m_shadowMap.AsDepthStencilView()));
		m_shadowPass.SetVertexShader(vertexShader);
		m_shadowPass.Init();
	}

	// normal_depth_pass
	{
		m_normalDepthMap.Init();

		//m_rarelyChangedData.shadowMapResolution = float(m_shadowMap.Resolution());

		std::shared_ptr<VertexShader> vertexShader = std::make_shared<VertexShader>(loader->LoadBinaryFile(GetRootDir().append(L"\\ssao_normal_depth_VS.cso")));
		vertexShader->SetVertexInput(std::make_shared<MeshDataVertexInput>());
		vertexShader->AddConstantBuffer(CBufferFrequency::per_object, std::make_unique<CBuffer<PerObjectData>>());

		std::shared_ptr<PixelShader> pixelShader = std::make_shared<PixelShader>(loader->LoadBinaryFile(GetRootDir().append(L"\\ssao_normal_depth_PS.cso")));
		pixelShader->AddConstantBuffer(CBufferFrequency::per_object, std::make_unique<CBuffer<PerObjectData>>());

		m_normalDepthPass.SetState(std::make_shared<RenderPassState>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_viewport, std::make_shared<SolidCullBackRS>(), m_normalDepthMap.AsRenderTargetView(), m_depthBufferView.Get()));
		m_normalDepthPass.SetVertexShader(vertexShader);
		m_normalDepthPass.SetPixelShader(pixelShader);
		m_normalDepthPass.Init();
	}

	m_randomVecMap.Init();
	
	//// SSAO pass
	{
		//m_SSAOMap.SetNoiseSize(4);
		//m_SSAOMap.SetKernelSize(32);
		//m_SSAOMap.SetPower(3.0f);
		//m_SSAOMap.SetRadius(1.69f);
		m_SSAOMap.Init();

		std::shared_ptr<VertexShader> vertexShader = std::make_shared<VertexShader>(loader->LoadBinaryFile(GetRootDir().append(L"\\ssao_map_VS.cso")));
		vertexShader->SetVertexInput(std::make_shared<VertexInputAmbientOcclusion>());
		vertexShader->AddConstantBuffer(CBufferFrequency::per_object, std::make_unique<CBuffer<PerObjectData>>());
		vertexShader->AddConstantBuffer(CBufferFrequency::per_object_ambient_occlusion, std::make_unique<CBuffer<PerObjectCBAmbientOcclusion>>());

		std::shared_ptr<PixelShader> pixelShader = std::make_shared<PixelShader>(loader->LoadBinaryFile(GetRootDir().append(L"\\ssao_map_PS.cso")));
		pixelShader->AddConstantBuffer(CBufferFrequency::per_object, std::make_unique<CBuffer<PerObjectData>>());
		pixelShader->AddConstantBuffer(CBufferFrequency::per_object_ambient_occlusion, std::make_unique<CBuffer<PerObjectCBAmbientOcclusion>>());
		pixelShader->AddSampler(SamplerUsage::normal_depth_map, std::make_shared<NormalDepthSampler>());
		pixelShader->AddSampler(SamplerUsage::random_vec, std::make_shared<RandomVecSampler>());

		m_SSAOPass.SetState(std::make_shared<RenderPassState>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_viewport, std::make_shared<SolidCullBackRS>(), m_SSAOMap.AsRenderTargetView(), nullptr)); // nullptr
		m_SSAOPass.SetVertexShader(vertexShader);
		m_SSAOPass.SetPixelShader(pixelShader);
		m_SSAOPass.Init();
	}
	

	// render pass
	{
		std::shared_ptr<VertexShader> vertexShader = std::make_shared<VertexShader>(loader->LoadBinaryFile(GetRootDir().append(L"\\SSAO_demo_VS.cso")));
		vertexShader->SetVertexInput(std::make_shared<MeshDataVertexInput>());
		vertexShader->AddConstantBuffer(CBufferFrequency::per_object, std::make_unique<CBuffer<PerObjectData>>());

		std::shared_ptr<PixelShader> pixelShader = std::make_shared<PixelShader>(loader->LoadBinaryFile(GetRootDir().append(L"\\SSAO_demo_PS.cso")));
		pixelShader->AddConstantBuffer(CBufferFrequency::per_object, std::make_unique<CBuffer<PerObjectData>>());
		pixelShader->AddConstantBuffer(CBufferFrequency::per_frame, std::make_unique<CBuffer<PerFrameData>>());
		pixelShader->AddConstantBuffer(CBufferFrequency::rarely_changed, std::make_unique<CBuffer<RarelyChangedData>>());
		pixelShader->AddSampler(SamplerUsage::common_textures, std::make_shared<AnisotropicSampler>());
		pixelShader->AddSampler(SamplerUsage::shadow_map, std::make_shared<PCFSampler>());
		pixelShader->AddSampler(SamplerUsage::ssao_map, std::make_shared<SSAOMapSampler>());

		m_renderPass.SetState(std::make_shared<RenderPassState>(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST, m_viewport, std::make_shared<SolidCullBackRS>(), m_backBufferView.Get(), m_depthBufferView.Get()));
		m_renderPass.SetVertexShader(vertexShader);
		m_renderPass.SetPixelShader(pixelShader);
		m_renderPass.Init();
	}
}


void SSAODemoApp::InitRenderables()
{
	render::Renderable ground{ *(service::Locator::GetResourceLoader()->LoadGPFMesh(GetRootDir().append(LR"(\3d-objects\rocks_dorama\rocks_composition.gpf)"))) };
	ground.SetTransform(XMMatrixScaling(1.5f, 1.5f, 1.5f) * XMMatrixTranslation(3.f, 0.f, 2.5f));
	ground.Init();
	m_objects.push_back(ground);

	render::Renderable soldier1{ *(service::Locator::GetResourceLoader()->LoadGPFMesh(GetRootDir().append(LR"(\3d-objects\gdc_female\gdc_female_posed_2.gpf)"))) };
	soldier1.SetTransform(XMMatrixRotationY(math::ToRadians(-12.f)) * XMMatrixTranslation(0.f, 0.4f, 0.f));
	soldier1.Init();
	m_objects.push_back(std::move(soldier1));

	render::Renderable soldier2{ *(service::Locator::GetResourceLoader()->LoadGPFMesh(GetRootDir().append(LR"(\3d-objects\gdc_female\gdc_female_posed.gpf)"))) };
	soldier2.SetTransform(XMMatrixRotationY(math::ToRadians(135.f)) * XMMatrixTranslation(10.f, 0.35f, -10.f));
	soldier2.Init();
	m_objects.push_back(std::move(soldier2));
}


void SSAODemoApp::OnResized()
{
	application::DirectxApp::OnResized();

	//update the render pass state with the resized render target and depth buffer
	m_renderPass.GetState()->ChangeRenderTargetView(m_backBufferView.Get());
	m_renderPass.GetState()->ChangeDepthStencilView(m_depthBufferView.Get());
	m_renderPass.GetState()->ChangeViewPort(m_viewport);

	//update the projection matrix with the new aspect ratio
	m_camera.SetPerspectiveProjection(math::ToRadians(45.f), AspectRatio(), 1.f, 1000.f);
}


void SSAODemoApp::OnWheelScroll(input::ScrollStatus scroll)
{
	// zoom in or out when the scroll wheel is used
	if (service::Locator::GetMouse()->IsInClientArea())
	{
		m_camera.IncreaseRadiusBy(scroll.isScrollingUp ? -0.5f : 0.5f);
	}
}


void SSAODemoApp::OnMouseMove(const DirectX::XMINT2& movement, const DirectX::XMINT2& currentPos)
{
	XTEST_UNUSED_VAR(currentPos);

	input::Mouse* mouse = service::Locator::GetMouse();

	// rotate the camera position around 
	if (mouse->GetButtonStatus(input::MouseButton::left_button).isDown && mouse->IsInClientArea())
	{
		m_camera.RotateBy(math::ToRadians(movement.y * -0.25f), math::ToRadians(movement.x * 0.25f));
	}

	// pan the camera 
	if (mouse->GetButtonStatus(input::MouseButton::right_button).isDown && mouse->IsInClientArea())
	{
		XMFLOAT3 cameraX = m_camera.GetXAxis();
		XMFLOAT3 cameraY = m_camera.GetYAxis();

		// we should calculate the right amount of pan in screen space but for now this is good enough
		XMVECTOR xPanTranslation = XMVectorScale(XMLoadFloat3(&cameraX), float(-movement.x) * 0.01f);
		XMVECTOR yPanTranslation = XMVectorScale(XMLoadFloat3(&cameraY), float(movement.y) * 0.01f);

		XMFLOAT3 panTranslation;
		XMStoreFloat3(&panTranslation, XMVectorAdd(xPanTranslation, yPanTranslation));
		m_camera.TranslatePivotBy(panTranslation);
	}

	// rotate the directional light
	if (mouse->GetButtonStatus(input::MouseButton::middle_button).isDown && mouse->IsInClientArea())
	{
		XMStoreFloat3(&m_dirKeyLight.dirW, XMVector3Rotate(XMLoadFloat3(&m_dirKeyLight.dirW), XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), math::ToRadians(movement.x * -0.25f))));
		XMStoreFloat3(&m_dirFillLight.dirW, XMVector3Rotate(XMLoadFloat3(&m_dirFillLight.dirW), XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), math::ToRadians(movement.x * -0.25f))));

		m_shadowMap.SetLight(m_dirKeyLight.dirW);
	}
}



void SSAODemoApp::OnKeyStatusChange(input::Key key, const input::KeyStatus& status)
{
	if (key == input::Key::F && status.isDown)
	{
		m_camera.SetPivot({ 5.f, 4.f, -5.f });
	}

	else if (key == input::Key::F1 && status.isDown)
	{
		m_rarelyChangedData.useShadowMap = !m_rarelyChangedData.useShadowMap;
		m_isRarelyChangedDataDirty = true;
	}

	if (key == input::Key::U && status.isDown)
	{
		m_SSAOMap.m_occlusionRadius += 0.1f;
	}

	if (key == input::Key::Y && status.isDown)
	{
		m_SSAOMap.m_occlusionRadius -= 0.1f;
	}

	if (key == input::Key::J && status.isDown)
	{
		m_SSAOMap.m_occlusionFadeStart += 0.1f;
	}

	if (key == input::Key::H && status.isDown)
	{
		m_SSAOMap.m_occlusionFadeStart -= 0.1f;
	}

	if (key == input::Key::M && status.isDown)
	{
		m_SSAOMap.m_occlusionFadeEnd += 0.1f;
	}

	if (key == input::Key::N && status.isDown)
	{
		m_SSAOMap.m_occlusionFadeEnd -= 0.1f;
	}

	if (key == input::Key::F2 && status.isDown)
	{
		m_rarelyChangedData.useSSAOMap = !m_rarelyChangedData.useSSAOMap;
		m_isRarelyChangedDataDirty = true;
	}
}

void SSAODemoApp::UpdateScene(float deltaSeconds)
{
	XTEST_UNUSED_VAR(deltaSeconds);

	// PerFrameCB
	{
		PerFrameData data;
		data.dirLights[0] = m_dirKeyLight;
		data.dirLights[1] = m_dirFillLight;
		data.eyePosW = m_camera.GetPosition();

		m_renderPass.GetPixelShader()->GetConstantBuffer(CBufferFrequency::per_frame)->UpdateBuffer(data);
	}


	// RarelyChangedCB
	if (m_isRarelyChangedDataDirty)
	{
		m_renderPass.GetPixelShader()->GetConstantBuffer(CBufferFrequency::rarely_changed)->UpdateBuffer(m_rarelyChangedData);
		m_isRarelyChangedDataDirty = false;
	}

}

void SSAODemoApp::RenderScene()
{

	m_d3dAnnotation->BeginEvent(L"shadow-map");
	m_shadowPass.Bind();
	m_shadowPass.GetState()->ClearDepthOnly();

	// draw objects
	for (render::Renderable& renderable : m_objects)
	{
		for (const std::string& meshName : renderable.GetMeshNames())
		{
			PerObjectShadowMapData data = ToPerObjectShadowMapData(renderable, meshName);
			m_shadowPass.GetVertexShader()->GetConstantBuffer(CBufferFrequency::per_object)->UpdateBuffer(data);
			renderable.Draw(meshName);
		}
	}
	m_d3dAnnotation->EndEvent();

	
	m_d3dAnnotation->BeginEvent(L"normal_depth-map");
	m_normalDepthPass.Bind();
	m_normalDepthPass.GetState()->ClearDepthOnly();
	m_normalDepthPass.GetState()->ClearRenderTarget(DirectX::Colors::Black);
	// draw objects
	for (render::Renderable& renderable : m_objects)
	{
		for (const std::string& meshName : renderable.GetMeshNames())
		{
			PerObjectData data = ToPerObjectData(renderable, meshName);
			m_normalDepthPass.GetVertexShader()->GetConstantBuffer(CBufferFrequency::per_object)->UpdateBuffer(data);
			renderable.Draw(meshName);
		}
	}
	m_d3dAnnotation->EndEvent();

	{
	m_d3dAnnotation->BeginEvent(L"ambient-occlusion-pass");
	m_SSAOPass.Bind();

	//m_SSAOPass.GetState()->ClearDepthOnly();
	m_SSAOPass.GetState()->ClearRenderTarget(DirectX::Colors::White);

	m_SSAOPass.GetPixelShader()->BindTexture(TextureUsage::normal_depth_map, m_normalDepthMap.AsShaderView());
	m_SSAOPass.GetPixelShader()->BindTexture(TextureUsage::random_vec_map, m_randomVecMap.AsShaderView());

	//attach CB
	PerObjectCBAmbientOcclusion data1 = ToPerObjectAmbientOcclusion();
	m_SSAOPass.GetVertexShader()->GetConstantBuffer(CBufferFrequency::per_object_ambient_occlusion)->UpdateBuffer(data1);
	m_SSAOPass.GetPixelShader()->GetConstantBuffer(CBufferFrequency::per_object_ambient_occlusion)->UpdateBuffer(data1);
	// compute MAP
	m_SSAOMap.Draw();
	}
	
	m_SSAOPass.GetPixelShader()->BindTexture(TextureUsage::normal_depth_map, nullptr);
	m_SSAOPass.GetPixelShader()->BindTexture(TextureUsage::random_vec_map, nullptr); // randomVecMap?
	m_d3dAnnotation->EndEvent();
	
	m_d3dAnnotation->BeginEvent(L"render-scene");
	m_renderPass.Bind();
	m_renderPass.GetState()->ClearDepthOnly();
	m_renderPass.GetState()->ClearRenderTarget(DirectX::Colors::DarkGray);
	m_renderPass.GetPixelShader()->BindTexture(TextureUsage::shadow_map, m_shadowMap.AsShaderView());
	m_renderPass.GetPixelShader()->BindTexture(TextureUsage::ssao_map, m_SSAOMap.AsShaderView());

	// draw objects
	for (render::Renderable& renderable : m_objects)
	{
		for (const std::string& meshName : renderable.GetMeshNames())
		{
			PerObjectData data = ToPerObjectData(renderable, meshName);
			m_renderPass.GetVertexShader()->GetConstantBuffer(CBufferFrequency::per_object)->UpdateBuffer(data);
			m_renderPass.GetPixelShader()->GetConstantBuffer(CBufferFrequency::per_object)->UpdateBuffer(data);
			m_renderPass.GetPixelShader()->BindTexture(TextureUsage::color, renderable.GetTextureView(TextureUsage::color, meshName));
			m_renderPass.GetPixelShader()->BindTexture(TextureUsage::normal, renderable.GetTextureView(TextureUsage::normal, meshName));
			m_renderPass.GetPixelShader()->BindTexture(TextureUsage::glossiness, renderable.GetTextureView(TextureUsage::glossiness, meshName));
			renderable.Draw(meshName);
		}
	}

	m_renderPass.GetPixelShader()->BindTexture(TextureUsage::shadow_map, nullptr); // explicit unbind the shadow map to suppress warning
	m_renderPass.GetPixelShader()->BindTexture(TextureUsage::ssao_map, nullptr);
	m_d3dAnnotation->EndEvent();

	XTEST_D3D_CHECK(m_swapChain->Present(0, 0));
}


SSAODemoApp::PerObjectData SSAODemoApp::ToPerObjectData(const render::Renderable& renderable, const std::string& meshName)
{
	PerObjectData data;

	XMMATRIX W = XMLoadFloat4x4(&renderable.GetTransform());
	XMMATRIX T = XMLoadFloat4x4(&renderable.GetTexcoordTransform(meshName));
	XMMATRIX V = m_camera.GetViewMatrix();
	XMMATRIX P = m_camera.GetProjectionMatrix();
	XMMATRIX WVP = W * V*P;
	XMMATRIX WVPT_shadowMap = W * m_shadowMap.VPTMatrix();


	static const XMMATRIX T1(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX WVPT_ssao = W * V * P * T1;

	XMStoreFloat4x4(&data.W, XMMatrixTranspose(W));
	XMStoreFloat4x4(&data.WVP, XMMatrixTranspose(WVP));
	XMStoreFloat4x4(&data.W_inverseTraspose, XMMatrixInverse(nullptr, W));
	XMStoreFloat4x4(&data.TexcoordMatrix, XMMatrixTranspose(T));
	XMStoreFloat4x4(&data.WVPT_shadowMap, XMMatrixTranspose(WVPT_shadowMap));
	XMStoreFloat4x4(&data.WVPT_ssao, XMMatrixTranspose(WVPT_ssao));

	data.material.ambient = renderable.GetMaterial(meshName).ambient;
	data.material.diffuse = renderable.GetMaterial(meshName).diffuse;
	data.material.specular = renderable.GetMaterial(meshName).specular;

	return data;
}

SSAODemoApp::PerObjectCBAmbientOcclusion SSAODemoApp::ToPerObjectAmbientOcclusion()
{
	BuildFrustumFarCorners(m_camera.GetYFov(), m_camera.GetZFarPlane());
	BuildOffsetVectors();
	PerObjectCBAmbientOcclusion data;

	XMMATRIX T(0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);//XMLoadFloat4x4(&renderable.GetTexcoordTransform(meshName));
	XMMATRIX P = m_camera.GetProjectionMatrix();
	XMMATRIX PT = XMMatrixMultiply(P,T);

	XMStoreFloat4x4(&data.viewToTexSpace, XMMatrixTranspose(PT));

	data.frustumCorners[0] = m_frustumFarCorner[0];
	data.frustumCorners[1] = m_frustumFarCorner[1];
	data.frustumCorners[2] = m_frustumFarCorner[2];
	data.frustumCorners[3] = m_frustumFarCorner[3];

	for (int i = 0; i < SSAOData::SAMPLE_COUNT; i++)
	{
		data.offsetVectors[i] = m_offsets[i];
	}

	data.occlusionFadeEnd = m_SSAOMap.m_occlusionFadeEnd;
	data.occlusionFadeStart = m_SSAOMap.m_occlusionFadeStart;
	data.occlusionRadius = m_SSAOMap.m_occlusionRadius;
	data.surfaceEpsilon = m_SSAOMap.m_surfaceEpsilon;

	return data;
}

SSAODemoApp::PerObjectShadowMapData SSAODemoApp::ToPerObjectShadowMapData(const render::Renderable& renderable, const std::string& meshName)
{
	XTEST_UNUSED_VAR(meshName);
	PerObjectShadowMapData data;

	XMMATRIX W = XMLoadFloat4x4(&renderable.GetTransform());
	XMMATRIX WVP = W * m_shadowMap.LightViewMatrix() * m_shadowMap.LightProjMatrix();

	XMStoreFloat4x4(&data.WVP_lightSpace, XMMatrixTranspose(WVP));
	return data;
}

void SSAODemoApp::BuildFrustumFarCorners(float fovy, float farZ)
{
	float aspect = m_camera.GetAspectRatio();
	float halfHeight = farZ * tanf(0.5f * fovy);
	float halfWidth = aspect * halfHeight;

	m_frustumFarCorner[0] = DirectX::XMFLOAT4(-halfWidth, -halfHeight, farZ, 0.0f);
	m_frustumFarCorner[1] = DirectX::XMFLOAT4(-halfWidth, +halfHeight, farZ, 0.0f);
	m_frustumFarCorner[2] = DirectX::XMFLOAT4(+halfWidth, +halfHeight, farZ, 0.0f);
	m_frustumFarCorner[3] = DirectX::XMFLOAT4(+halfWidth, -halfHeight, farZ, 0.0f);

}

float RandomFloat(float min, float max)
{
	
	float scale = rand() / (float)RAND_MAX; /* [0, 1.0] */
	return min + scale * (max - min);      /* [min, max] */
}

void SSAODemoApp::BuildOffsetVectors()
{
	m_offsets[0] = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
	m_offsets[1] = DirectX::XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.0f);
	m_offsets[2] = DirectX::XMFLOAT4(-1.0f, 1.0f, 1.0f, 0.0f);
	m_offsets[3] = DirectX::XMFLOAT4(1.0f, -1.0f, -1.0f, 0.0f);
	m_offsets[4] = DirectX::XMFLOAT4(1.0f, 1.0f, -1.0f, 0.0f);
	m_offsets[5] = DirectX::XMFLOAT4(-1.0f, -1.0f, 1.0f, 0.0f);
	m_offsets[6] = DirectX::XMFLOAT4(-1.0f, 1.0f, -1.0f, 0.0f);
	m_offsets[7] = DirectX::XMFLOAT4(1.0f, -1.0f, 1.0f, 0.0f);
	m_offsets[8] = DirectX::XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
	m_offsets[9] = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	m_offsets[10] = DirectX::XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	m_offsets[11] = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	m_offsets[12] = DirectX::XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
	m_offsets[13] = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
	
	for (int i = 14; i < SSAOData::SAMPLE_COUNT; i++) 
	{
		m_offsets[i] = DirectX::XMFLOAT4(RandomFloat(0.25f, 1.0f), RandomFloat(0.25f, 1.0f), RandomFloat(0.25f, 1.0f), RandomFloat(0.25f, 1.0f));
	}

	srand((unsigned) std::time(NULL));
	for (int i = 0; i < SSAOData::SAMPLE_COUNT; i++)
	{
		float s = RandomFloat(0.25f, 1.0f);
		XMVECTOR v = s * XMVector4Normalize(XMLoadFloat4(&m_offsets[i]));
		XMStoreFloat4(&m_offsets[i], v);
	}
}
