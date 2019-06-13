#pragma once
#include <render/render_resource.h>

namespace xtest {
	namespace render {
		namespace shading {



			struct SSAOData
			{

				struct VertexInAmbientOcclusion
				{

					VertexInAmbientOcclusion(DirectX::XMFLOAT3, DirectX::XMFLOAT3, DirectX::XMFLOAT2);

					DirectX::XMFLOAT3 pos;
					DirectX::XMFLOAT3 toFarPlaneIndex;
					DirectX::XMFLOAT2 uv;

					bool operator==(const VertexInAmbientOcclusion& other) const;
				};

				std::vector<VertexInAmbientOcclusion> vertices;
				std::vector<uint32> indices;
				static const int SAMPLE_COUNT = 256;
			};

			// class representing a shadow map
			class BlurMap
			{
			public:
				/**
				 * @param width Resolution for SSAO map
				 * @param height Resolution for SSAO map
				 * @param sample_factor Divide width and height by this factor
				 * @param kernel_size how many sample take in the hemisphere
				 * @param noise_size
				 * @param radius sphere radius
				 * @param how strong is SSAO power
				 */
				explicit BlurMap(
					uint32 width,
					uint32 height
				);

				void Init();

				/*void SetNoiseSize(uint32 noise_size);
				void SetKernelSize(uint32 kernel_size);
				void SetRadius(float radius);
				void SetPower(float power);*/

				ID3D11ShaderResourceView* AsShaderView();
				ID3D11RenderTargetView* AsRenderTargetView();
				D3D11_VIEWPORT Viewport() const;
				void SetViewport(D3D11_VIEWPORT& view);
				void ResetTargetView(uint32 n_width, uint32 n_height);

				uint32 Width() const;
				uint32 Height() const;
				static float RandomFloat(float, float);
				static float lerp(float, float, float);
				void BuildKernelVectors();
				void BuildFrustumFarCorners(float aspect, float fovy, float farZ);
				DirectX::XMFLOAT4* GetFrustumFarCorner();
				DirectX::XMFLOAT4* GetKernelVectors();

				void Bind();
				void Draw();

				/*uint32 NoiseSize() const;
				uint32 KernelSize() const;
				float Radius() const;
				float Power() const;*/

				float m_occlusionRadius = 0.5f;
				float m_occlusionFadeStart = 0.2f;
				float m_occlusionFadeEnd = 2.0f;
				float m_surfaceEpsilon = 0.0005f;
				float m_multiplier = 4.0f;

			private:
				SSAOData m_vs_data;
				Microsoft::WRL::ComPtr<ID3D11Buffer> m_d3dVertexBuffer;
				Microsoft::WRL::ComPtr<ID3D11Buffer> m_d3dIndexBuffer;

				uint32 m_height;
				uint32 m_width;

				DirectX::XMFLOAT4 m_frustumFarCorner[4];
				DirectX::XMFLOAT4 m_offsets[xtest::render::shading::SSAOData::SAMPLE_COUNT];
				/*uint32 m_noise_size;
				uint32 m_kernel_size;
				float m_radius;
				float m_power;*/

				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderView;
				Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
				D3D11_VIEWPORT m_viewport;
			};
		} //shading
	} //render
} //xtest
