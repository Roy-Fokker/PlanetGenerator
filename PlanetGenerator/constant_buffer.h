#pragma once

#include "direct3d.h"
#include <winrt/base.h>
#include <d3d11_1.h>


namespace planet_generator
{
	struct transforms;

	enum class shader_stage
	{
		vertex,
		pixel
	};

	enum class shader_slot
	{
		projection = 0,
		view = 1,
		transform = 2
	};

	class constant_buffer
	{
	public:
		using buffer_t = winrt::com_ptr<ID3D11Buffer>;

		

	public:
		constant_buffer() = delete;
		constant_buffer(direct3d::device_t device, const transforms &data, shader_slot slot);
		~constant_buffer();

		void activate(direct3d::context_t context);
		void update(direct3d::context_t context, const transforms &data);

	private:
		void make_buffer(direct3d::device_t device, const transforms &data);

	private:
		shader_slot slot;
		buffer_t buffer;
		shader_stage p_stage = shader_stage::vertex;
	};
}

