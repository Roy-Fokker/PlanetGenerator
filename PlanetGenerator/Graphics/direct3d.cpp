#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")

#ifdef _DEBUG
#pragma comment(lib, "dxguid.lib")
#endif // DEBUG

#include "direct3d.h"

#include <cassert>
#include <cstdint>

using namespace planet_generator;

namespace
{
	constexpr DXGI_FORMAT swap_chain_format = DXGI_FORMAT_R8G8B8A8_UNORM;
	constexpr uint16_t msaa_quality_level = 4U;
	//constexpr uint32_t max_anisotropy = 16U;

	const std::array<uint16_t, 2> get_window_size(HWND window_handle)
	{
		RECT rect{};
		GetClientRect(window_handle, &rect);

		return {
			static_cast<uint16_t>(rect.right - rect.left),
			static_cast<uint16_t>(rect.bottom - rect.top)
		};
	}

	const std::array<uint16_t, 2> get_swap_chain_size(direct3d::swap_chain_t swap_chain)
	{
		DXGI_SWAP_CHAIN_DESC sd{};
		auto hr = swap_chain->GetDesc(&sd);
		assert(hr == S_OK);

		return {
			static_cast<uint16_t>(sd.BufferDesc.Width),
			static_cast<uint16_t>(sd.BufferDesc.Height)
		};
	}

	const DXGI_RATIONAL get_refresh_rate(winrt::com_ptr<IDXGIAdapter> dxgiAdapter, HWND window_handle, bool vSync = true)
	{
		DXGI_RATIONAL refresh_rate{ 0, 1 };

		if (vSync)
		{
			HRESULT hr{};

			winrt::com_ptr<IDXGIOutput> adapter_output;
			hr = dxgiAdapter->EnumOutputs(0, adapter_output.put());
			assert(hr == S_OK);

			uint32_t display_modes_count{ 0 };
			hr = adapter_output->GetDisplayModeList(swap_chain_format,
													DXGI_ENUM_MODES_INTERLACED,
													&display_modes_count,
													nullptr);
			assert(hr == S_OK);


			std::vector<DXGI_MODE_DESC> display_modes(display_modes_count);
			hr = adapter_output->GetDisplayModeList(swap_chain_format,
													DXGI_ENUM_MODES_INTERLACED,
													&display_modes_count,
													&display_modes[0]);
			assert(hr == S_OK);

			auto[width, height] = get_window_size(window_handle);

			for (auto &mode : display_modes)
			{
				if (mode.Width == width && mode.Height == height)
				{
					refresh_rate = mode.RefreshRate;
				}
			}
		}

		return refresh_rate;
	}
}

const DXGI_SAMPLE_DESC direct3d::get_msaa_level(direct3d::device_t device)
{
	DXGI_SAMPLE_DESC sd{ 1, 0 };

#ifdef _DEBUG
	return sd;
#endif

	UINT msaa_level{ 0 };

	auto hr = device->CheckMultisampleQualityLevels(swap_chain_format, msaa_quality_level, &msaa_level);
	assert(hr == S_OK);

	if (msaa_level > 0)
	{
		sd.Count = msaa_quality_level;
		sd.Quality = msaa_level - 1;
	}

	return sd;
}

direct3d::direct3d(HWND hWnd) :
	window_handle(hWnd)
{
	make_device();
	make_swap_chain();
}

direct3d::~direct3d() = default;

void direct3d::resize()
{
	auto hr = swap_chain->ResizeBuffers(NULL, NULL, NULL, DXGI_FORMAT_UNKNOWN, NULL);
	assert(hr == S_OK);
}

void direct3d::present(bool vSync)
{
	swap_chain->Present((vSync ? TRUE : FALSE), NULL);
}

void direct3d::make_device()
{
	uint32_t flags{};
	flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT; // Needed for Direct 2D;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

	std::array<D3D_FEATURE_LEVEL, 1> feature_levels{
		D3D_FEATURE_LEVEL_11_0
	};

	auto hr = D3D11CreateDevice(nullptr,
								D3D_DRIVER_TYPE_HARDWARE,
								nullptr,
								flags,
								feature_levels.data(),
								static_cast<uint32_t>(feature_levels.size()),
								D3D11_SDK_VERSION,
								device.put(),
								nullptr,
								context.put());
	assert(hr == S_OK);
}

void direct3d::make_swap_chain()
{
	HRESULT hr{};

	winrt::com_ptr<IDXGIDevice> dxgi_device{};
	hr = device->QueryInterface<IDXGIDevice>(dxgi_device.put());
	assert(hr == S_OK);

	winrt::com_ptr<IDXGIAdapter> dxgi_adapter{};
	hr = dxgi_device->GetAdapter(dxgi_adapter.put());
	//hr = dxgi_device->GetParent(__uuidof(IDXGIAdapter), dxgi_adapter.put_void()));
	assert(hr == S_OK);

	winrt::com_ptr<IDXGIFactory> dxgi_factory{};
	hr = dxgi_adapter->GetParent(__uuidof(IDXGIFactory), dxgi_factory.put_void());
	assert(hr == S_OK);

	auto[width, height] = get_window_size(window_handle);

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferCount = 1;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = swap_chain_format;
	sd.BufferDesc.RefreshRate = get_refresh_rate(dxgi_adapter, window_handle);
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = window_handle;
	sd.SampleDesc = get_msaa_level(device);
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.Windowed = TRUE;

	hr = dxgi_factory->CreateSwapChain(device.get(), &sd, swap_chain.put());
	assert(hr == S_OK);

	dxgi_factory->MakeWindowAssociation(window_handle, DXGI_MWA_NO_ALT_ENTER | DXGI_MWA_NO_WINDOW_CHANGES);
}

