#pragma once

#include <Windows.h>
#include <winrt/base.h>
#include <d3d11_1.h>
#include <dxgi1_2.h>

namespace planet_generator
{
	class direct3d
	{
	public:
		using device_t = winrt::com_ptr<ID3D11Device>;
		using swap_chain_t = winrt::com_ptr<IDXGISwapChain>;
		using context_t = winrt::com_ptr<ID3D11DeviceContext>;

	public:
		static DXGI_SAMPLE_DESC const get_msaa_level(direct3d::device_t device);

	public:
		direct3d() = delete;
		direct3d(HWND hWnd);
		~direct3d();

		void resize();
		void present(bool vSync = false);

		template<typename T>
		T as() const
		{
			if constexpr (std::is_same<T, device_t>())
			{
				return device;
			}
			else if constexpr (std::is_same<T, swap_chain_t>())
			{
				return swap_chain;
			}
			else if constexpr (std::is_same<T, context_t>())
			{
				return context;
			}
			else
			{
				static_assert(false, "Must be one of [device_t, swap_chain_t, context_t]");
			}
		};

	private:
		void make_device();
		void make_swap_chain();

	private:
		device_t device;
		swap_chain_t swap_chain;
		context_t context;

		HWND window_handle;
	};
}