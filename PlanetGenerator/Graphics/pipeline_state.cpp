#include "pipeline_state.h"
#include <DirectXColors.h>

using namespace planet_generator;

namespace
{
	constexpr uint32_t max_anisotropy = 16U;
}

pipeline_state::pipeline_state(direct3d::device_t device, const description & state_desc)
{
	make_blend_state(device, state_desc.blend);
	make_depth_stencil_state(device, state_desc.depth_stencil);
	make_rasterizer_state(device, state_desc.rasterizer);
	make_sampler_state(device, state_desc.sampler);

	primitive_topology = state_desc.primitive_topology;
}

pipeline_state::~pipeline_state() = default;

void pipeline_state::activate(direct3d::context_t context)
{
	context->OMSetBlendState(blend_state.get(),
	                         DirectX::Colors::Transparent,
	                         0xffff'ffff);
	context->OMSetDepthStencilState(depth_stencil_state.get(),
	                                NULL);
	context->RSSetState(rasterizer_state.get());

	ID3D11SamplerState * const samplers[] = { sampler_state.get() };
	context->PSSetSamplers(0,
	                       1,
	                       samplers);


	context->IASetPrimitiveTopology(primitive_topology);
}

void pipeline_state::make_blend_state(direct3d::device_t device, blend_mode blend)
{
	D3D11_BLEND src{}, dst{};
	D3D11_BLEND_OP op{ D3D11_BLEND_OP_ADD };

	switch (blend)
	{
	case blend_mode::Opaque:
		src = D3D11_BLEND_ONE;
		dst = D3D11_BLEND_ZERO;
		break;
	case blend_mode::Alpha:
		src = D3D11_BLEND_ONE;
		dst = D3D11_BLEND_INV_SRC_ALPHA;
		break;
	case blend_mode::Additive:
		src = D3D11_BLEND_SRC_ALPHA;
		dst = D3D11_BLEND_ONE;
		break;
	case blend_mode::NonPremultipled:
		src = D3D11_BLEND_SRC_ALPHA;
		dst = D3D11_BLEND_INV_SRC_ALPHA;
		break;
	}

	D3D11_BLEND_DESC bd{};

	bd.RenderTarget[0].BlendEnable = ((src != D3D11_BLEND_ONE) || (dst != D3D11_BLEND_ONE));

	bd.RenderTarget[0].SrcBlend = src;
	bd.RenderTarget[0].BlendOp = op;
	bd.RenderTarget[0].DestBlend = dst;

	bd.RenderTarget[0].SrcBlendAlpha = src;
	bd.RenderTarget[0].BlendOpAlpha = op;
	bd.RenderTarget[0].DestBlendAlpha = dst;

	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	auto hr = device->CreateBlendState(&bd, blend_state.put());
	assert(hr == S_OK);
}

void pipeline_state::make_depth_stencil_state(direct3d::device_t device, depth_stencil_mode depth_stencil)
{
	bool depth_enable{ false }, write_enable{ false };

	switch (depth_stencil)
	{
	case depth_stencil_mode::None:
		depth_enable = false;
		write_enable = false;
		break;
	case depth_stencil_mode::ReadWrite:
		depth_enable = true;
		write_enable = true;
		break;
	case depth_stencil_mode::ReadOnly:
		depth_enable = true;
		write_enable = false;
		break;
	}

	D3D11_DEPTH_STENCIL_DESC dsd{};

	dsd.DepthEnable = depth_enable;
	dsd.DepthWriteMask = write_enable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	dsd.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	dsd.StencilEnable = false;
	dsd.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsd.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

	dsd.BackFace = dsd.FrontFace;

	auto hr = device->CreateDepthStencilState(&dsd, depth_stencil_state.put());
	assert(hr == S_OK);
}

void pipeline_state::make_rasterizer_state(direct3d::device_t device, rasterizer_mode rasterizer)
{
	D3D11_CULL_MODE cull_mode{};
	D3D11_FILL_MODE fill_mode{};

	switch (rasterizer)
	{
	case rasterizer_mode::CullNone:
		cull_mode = D3D11_CULL_NONE;
		fill_mode = D3D11_FILL_SOLID;
		break;
	case rasterizer_mode::CullClockwise:
		cull_mode = D3D11_CULL_FRONT;
		fill_mode = D3D11_FILL_SOLID;
		break;
	case rasterizer_mode::CullAntiClockwise:
		cull_mode = D3D11_CULL_BACK;
		fill_mode = D3D11_FILL_SOLID;
		break;
	case rasterizer_mode::Wireframe:
		cull_mode = D3D11_CULL_BACK;
		fill_mode = D3D11_FILL_WIREFRAME;
		break;
	}

	D3D11_RASTERIZER_DESC rd{};

	rd.CullMode = cull_mode;
	rd.FillMode = fill_mode;
	rd.DepthClipEnable = true;
	rd.MultisampleEnable = true;


	auto hr = device->CreateRasterizerState(&rd, rasterizer_state.put());
	assert(hr == S_OK);
}

void pipeline_state::make_sampler_state(direct3d::device_t device, sampler_mode sampler)
{
	D3D11_FILTER filter{};
	D3D11_TEXTURE_ADDRESS_MODE texture_address_mode{};

	switch (sampler)
	{
	case sampler_mode::PointWrap:
		filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		texture_address_mode = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case sampler_mode::PointClamp:
		filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		texture_address_mode = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	case sampler_mode::LinearWrap:
		filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		texture_address_mode = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case sampler_mode::LinearClamp:
		filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		texture_address_mode = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	case sampler_mode::AnisotropicWrap:
		filter = D3D11_FILTER_ANISOTROPIC;
		texture_address_mode = D3D11_TEXTURE_ADDRESS_WRAP;
		break;
	case sampler_mode::AnisotropicClamp:
		filter = D3D11_FILTER_ANISOTROPIC;
		texture_address_mode = D3D11_TEXTURE_ADDRESS_CLAMP;
		break;
	}

	D3D11_SAMPLER_DESC sd{ };

	sd.Filter = filter;

	sd.AddressU = texture_address_mode;
	sd.AddressV = texture_address_mode;
	sd.AddressW = texture_address_mode;

	sd.MaxAnisotropy = max_anisotropy;

	sd.MaxLOD = FLT_MAX;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;

	auto hr = device->CreateSamplerState(&sd, sampler_state.put());
	assert(hr == S_OK);
}

