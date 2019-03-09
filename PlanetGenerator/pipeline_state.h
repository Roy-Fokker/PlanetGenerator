#pragma once

#include "direct3d.h"
#include <winrt/base.h>
#include <vector>

namespace planet_generator
{
	class pipeline_state
	{
	public:
		using blend_state_t = winrt::com_ptr<ID3D11BlendState>;
		using depth_stencil_state_t = winrt::com_ptr<ID3D11DepthStencilState>;
		using rasterizer_state_t = winrt::com_ptr<ID3D11RasterizerState>;
		using sampler_state_t = winrt::com_ptr<ID3D11SamplerState>;

		using vertex_shader_t = winrt::com_ptr<ID3D11VertexShader>;
		using pixel_shader_t = winrt::com_ptr<ID3D11PixelShader>;
		using input_layout_t = winrt::com_ptr<ID3D11InputLayout>;

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

		enum class input_layout_mode
		{
			position,
			//position_texcoord
		};

		struct description
		{
			blend_mode blend;
			depth_stencil_mode depth_stencil;
			rasterizer_mode rasterizer;
			sampler_mode sampler;

			D3D11_PRIMITIVE_TOPOLOGY primitive_topology;
			input_layout_mode input_layout;
			const std::vector<byte> &vertex_shader_file;
			const std::vector<byte> &pixel_shader_file;
		};

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

		void make_input_layout(direct3d::device_t device, input_layout_mode input_layout, const std::vector<byte> &vso);
		void make_vertex_shader(direct3d::device_t device, const std::vector<byte> &vso);
		void make_pixel_shader(direct3d::device_t device, const std::vector<byte> &pso);

	private:
		blend_state_t blend_state;
		depth_stencil_state_t depth_stencil_state;
		rasterizer_state_t rasterizer_state;
		sampler_state_t sampler_state;

		D3D11_PRIMITIVE_TOPOLOGY primitive_topology;
		input_layout_t input_layout;
		vertex_shader_t vertex_shader;
		pixel_shader_t pixel_shader;
	};
}

