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
				static const int SAMPLE_COUNT=128;
			};

			// class representing a shadow map
			class SSAOMap
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
				explicit SSAOMap(
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
				uint32 Width() const;
				uint32 Height() const;
				void Bind();
				void Draw();

				/*uint32 NoiseSize() const;
				uint32 KernelSize() const;
				float Radius() const;
				float Power() const;*/

				float m_occlusionRadius = 0.5f;
				float m_occlusionFadeStart = 0.2f;
				float m_occlusionFadeEnd = 2.0f;
				float m_surfaceEpsilon = 0.05f;

			private:
				SSAOData m_vs_data;
				Microsoft::WRL::ComPtr<ID3D11Buffer> m_d3dVertexBuffer;
				Microsoft::WRL::ComPtr<ID3D11Buffer> m_d3dIndexBuffer;

				uint32 m_height;
				uint32 m_width;

				
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
