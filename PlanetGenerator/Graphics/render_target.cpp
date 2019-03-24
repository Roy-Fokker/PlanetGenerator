#include "render_target.h"

using namespace planet_generator;

namespace
{
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
}

render_target::render_target(direct3d::device_t device, direct3d::swap_chain_t swap_chain)
{
	auto[width, height] = get_swap_chain_size(swap_chain);

	make_target_view(device, swap_chain);
	make_stencil_view(device, { width, height });

	viewport = {};
	viewport.Width = width;
	viewport.Height = height;
	viewport.MaxDepth = 1.0f;
}

render_target::~render_target() = default;

void render_target::activate(direct3d::context_t context)
{
	ID3D11RenderTargetView * const targets[] = { target_view.get() };
	context->OMSetRenderTargets(1, targets, stencil_view.get());
	context->RSSetViewports(1, &viewport);
}

void render_target::clear(direct3d::context_t context, const std::array<float, 4>& clear_color)
{
	context->ClearRenderTargetView(target_view.get(), &clear_color[0]);
	context->ClearDepthStencilView(stencil_view.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void render_target::make_target_view(direct3d::device_t device, direct3d::swap_chain_t swap_chain)
{
	texture_2d_t buffer = nullptr;
	auto hr = swap_chain->GetBuffer(0,
	                                __uuidof(ID3D11Texture2D),
	                                buffer.put_void());
	assert(hr == S_OK);

	hr = device->CreateRenderTargetView(buffer.get(),
	                                    0,
	                                    target_view.put());
	assert(hr == S_OK);
}

void render_target::make_stencil_view(direct3d::device_t device, const std::array<uint16_t, 2>& buffer_size)
{
	auto[width, height] = buffer_size;

	D3D11_TEXTURE2D_DESC td{};
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	td.SampleDesc = direct3d::get_msaa_level(device);

	auto hr = device->CreateTexture2D(&td,
	                                  0,
	                                  depth_buffer.put());
	assert(hr == S_OK);

	hr = device->CreateDepthStencilView(depth_buffer.get(),
	                                    0,
	                                    stencil_view.put());
	assert(hr == S_OK);
}
