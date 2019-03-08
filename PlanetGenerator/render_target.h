#pragma once

#include "direct3d.h"
#include <winrt/base.h>
#include <array>

namespace planet_generator
{

	class render_target
	{
	public:
		using target_view_t = winrt::com_ptr<ID3D11RenderTargetView>;
		using stencil_view_t = winrt::com_ptr<ID3D11DepthStencilView>;
		using texture_2d_t = winrt::com_ptr<ID3D11Texture2D>;

	public:
		render_target() = delete;
		render_target(direct3d::device_t device, direct3d::swap_chain_t swap_chain);
		~render_target();

		void activate(direct3d::context_t context);
		void clear(direct3d::context_t context, const std::array<float, 4> &clear_color);

	private:
		void make_target_view(direct3d::device_t device, direct3d::swap_chain_t swap_chain);
		void make_stencil_view(direct3d::device_t device, const std::array<uint16_t, 2> &buffer_size);

	private:
		target_view_t target_view;
		texture_2d_t depth_buffer;
		stencil_view_t stencil_view;
		D3D11_VIEWPORT viewport;
	};

}
