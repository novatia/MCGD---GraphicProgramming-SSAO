#pragma once

namespace xtest {
namespace render {

	enum class OutputModeOrder
	{
		high_refresh_first,
		low_refresh_first
	};

	Microsoft::WRL::ComPtr<IDXGIOutput> PrimaryOutput(Microsoft::WRL::ComPtr<ID3D11Device> d3dDevice);

	std::vector<DXGI_MODE_DESC> OutputModesByRatio(Microsoft::WRL::ComPtr<IDXGIOutput> d3dOutput, float ratio);

	std::vector<DXGI_MODE_DESC> BestMatchOutputModes(Microsoft::WRL::ComPtr<IDXGIOutput> d3dOutput, UINT desiredWidth, UINT desiredHeight, OutputModeOrder ordering);




} // render
} // xtest

