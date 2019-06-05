#pragma once
#include <wrl\implements.h>
#include <d3d11.h>
#include "SSAO_demo_app.h"
#include "common\assert.h"

namespace xtest {
	namespace bravo {
		///
		/// @brief Pass responsible to generate ambient accessibility buffer (for ambient occlusion)
		/// 
		class AmbientOcclusionPass {
		public:
			AmbientOcclusionPass() = default;
			~AmbientOcclusionPass() = default;
			AmbientOcclusionPass(const AmbientOcclusionPass&) = delete;
			const AmbientOcclusionPass& operator=(const AmbientOcclusionPass&) = delete;
			AmbientOcclusionPass(AmbientOcclusionPass&&) = delete;
			AmbientOcclusionPass& operator=(AmbientOcclusionPass&&) = delete;

			///
			/// @brief Initializes the pass
			/// @param normalRoughnessBuffer Geometry buffer that contains normals and roughness factors.
			/// @param depthBuffer Depth buffer
			/// @param normalRoughnessBufferShaderResourceView Shader resource view to
			/// the normal and roughness buffer
			/// @param depthBufferShaderResourceView Shader resource view to the depth buffer
			///
			void Init() noexcept;

				///
				/// @brief Executes the pass
				///
				/// Init() must be called first. This method can record and
				/// push command lists to the CommandListExecutor.
				///
				/// @param frameCBuffer Constant buffer per frame, for current frame
				/// @return The number of recorded command lists.
				///
				std::uint32_t Execute(const xtest::demo::SSAODemoApp::PerObjectData& frameCBuffer) noexcept;

				///
				/// @brief Get the ambient accessibility buffer. This is necessary for other passes.
				/// @return Ambient accessibility buffer
				///
				ID3D11Resource& GetAmbientAccessibilityBuffer() noexcept
				{
					return *(m_BlurBuffer.Get());
				}
		
		///
		/// @brief Get ambient accessibility buffer shader resource view. This is necessary for other passes
		/// @return Ambient accessibility buffer shader resource view
		///
		ID3D11ShaderResourceView& GetAmbientAccessibilityShaderResourceView() noexcept
		{
			return *(m_BlurBufferShaderResourceView.Get());
		}

		private:
			///
			/// @brief Checks if internal data is valid. Typically, used for assertions
			/// @return True if valid. Otherwise, false
			///
			bool IsDataValid() const noexcept;


			Microsoft::WRL::ComPtr<ID3D11Resource> m_AmbientAccessibilityBuffer;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_AmbientAccessibilityBufferShaderResourceView;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_AmbientAccessibilityBufferRenderTargetView;

			Microsoft::WRL::ComPtr<ID3D11Resource> m_BlurBuffer;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_BlurBufferShaderResourceView;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_BlurBufferRenderTargetView;

			Microsoft::WRL::ComPtr<ID3D11Resource> m_NormalRoughnessBuffer;
			Microsoft::WRL::ComPtr<ID3D11Resource> m_DepthBuffer;
		};
	}
}