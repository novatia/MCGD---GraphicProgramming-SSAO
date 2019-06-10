#pragma once
#include <render/render_resource.h>

namespace xtest {
	namespace render {
		namespace shading {

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
				ID3D11DepthStencilView* AsDepthStencilView();
				D3D11_VIEWPORT Viewport() const;
				uint32 Width() const;
				uint32 Height() const;
				/*uint32 NoiseSize() const;
				uint32 KernelSize() const;
				float Radius() const;
				float Power() const;*/

			private:
				uint32 m_height;
				uint32 m_width;
				/*uint32 m_noise_size;
				uint32 m_kernel_size;
				float m_radius;
				float m_power;*/

				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderView;
				Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;
				D3D11_VIEWPORT m_viewport;
			};
		} //shading
	} //render
} //xtest
