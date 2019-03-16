#pragma once

#include "direct3d.h"
#include <winrt/base.h>
#include <vector>

namespace planet_generator
{
	struct material_description;

	class material
	{
	public:
		using vertex_shader_t = winrt::com_ptr<ID3D11VertexShader>;
		using pixel_shader_t = winrt::com_ptr<ID3D11PixelShader>;
		using input_layout_t = winrt::com_ptr<ID3D11InputLayout>;

		enum class input_layout_mode
		{
			position,
			normal,
			texcoord
		};

		using description = material_description;

	public:
		material() = delete;
		material(direct3d::device_t device, const description &mat_desc);
		~material();

		void activate(direct3d::context_t context);

	private:
		void make_input_layout(direct3d::device_t device, const std::vector<material::input_layout_mode> input_layout, const std::vector<byte> &vso);
		void make_vertex_shader(direct3d::device_t device, const std::vector<byte> &vso);
		void make_pixel_shader(direct3d::device_t device, const std::vector<byte> &pso);

	private:
		input_layout_t input_layout;
		vertex_shader_t vertex_shader;
		pixel_shader_t pixel_shader;
	};

	struct material_description
	{
		const std::vector<material::input_layout_mode> &input_layout;
		const std::vector<byte> &vertex_shader_file;
		const std::vector<byte> &pixel_shader_file;
	};
}
