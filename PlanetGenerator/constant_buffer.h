#pragma once

#include "direct3d.h"
#include <winrt/base.h>
#include <d3d11_1.h>


namespace planet_generator
{
	struct transforms;

	class constant_buffer
	{
	public:
		using buffer_t = winrt::com_ptr<ID3D11Buffer>;

		enum class stage
		{
			vertex,
			pixel
		};

	public:
		constant_buffer() = delete;
		constant_buffer(direct3d::device_t device, const transforms &data, uint16_t slot);
		~constant_buffer();

		void activate(direct3d::context_t context);
		void update(direct3d::context_t context, const transforms &data);

	private:
		void make_buffer(direct3d::device_t device, const transforms &data);

	private:
		uint16_t slot;
		buffer_t buffer;
		stage p_stage = stage::vertex;
	};
}

