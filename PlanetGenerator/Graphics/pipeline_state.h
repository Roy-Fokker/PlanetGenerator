#pragma once

#include "direct3d.h"
#include <winrt/base.h>
#include <vector>

namespace planet_generator
{
	struct pipeline_description;

	class pipeline_state
	{
	public:
		using blend_state_t = winrt::com_ptr<ID3D11BlendState>;
		using depth_stencil_state_t = winrt::com_ptr<ID3D11DepthStencilState>;
		using rasterizer_state_t = winrt::com_ptr<ID3D11RasterizerState>;
		using sampler_state_t = winrt::com_ptr<ID3D11SamplerState>;

	public:
		enum class blend_mode
		{
			Opaque,
			Alpha,
			Additive,
			NonPremultipled
		};

		enum class depth_stencil_mode
		{
			None,
			ReadWrite,
			ReadOnly
		};

		enum class rasterizer_mode
		{
			CullNone,
			CullClockwise,
			CullAntiClockwise,
			Wireframe
		};

		enum class sampler_mode
		{
			PointWrap,
			PointClamp,
			LinearWrap,
			LinearClamp,
			AnisotropicWrap,
			AnisotropicClamp
		};

		using description = pipeline_description;

	public:
		pipeline_state() = delete;
		pipeline_state(direct3d::device_t device, const description &state_desc);
		~pipeline_state();

		void activate(direct3d::context_t context);

	private:
		void make_blend_state(direct3d::device_t device, blend_mode blend);
		void make_depth_stencil_state(direct3d::device_t device, depth_stencil_mode depth_stencil);
		void make_rasterizer_state(direct3d::device_t device, rasterizer_mode rasterizer);
		void make_sampler_state(direct3d::device_t device, sampler_mode sampler);

	private:
		blend_state_t blend_state;
		depth_stencil_state_t depth_stencil_state;
		rasterizer_state_t rasterizer_state;
		sampler_state_t sampler_state;

		D3D11_PRIMITIVE_TOPOLOGY primitive_topology;
	};

	struct pipeline_description
	{
		pipeline_state::blend_mode blend;
		pipeline_state::depth_stencil_mode depth_stencil;
		pipeline_state::rasterizer_mode rasterizer;
		pipeline_state::sampler_mode sampler;

		D3D11_PRIMITIVE_TOPOLOGY primitive_topology;
	};
}

