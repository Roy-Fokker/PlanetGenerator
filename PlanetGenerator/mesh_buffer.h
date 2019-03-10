#pragma once

#include "direct3d.h"
#include <winrt/base.h>
#include <DirectXMath.h>
#include <cstdint>
#include <vector>


namespace planet_generator
{
	struct vertex
	{
		DirectX::XMFLOAT3 position;
	};

	struct mesh
	{
		std::vector<vertex> verticies;
		std::vector<uint32_t> indicies;
	};

	struct transforms
	{
		DirectX::XMMATRIX data;
	};

	class mesh_buffer
	{
	public:
		using buffer_t = winrt::com_ptr<ID3D11Buffer>;

	public:
		mesh_buffer() = delete;
		mesh_buffer(direct3d::device_t device, const std::vector<vertex> &vertices, const std::vector<uint32_t> &indicies);
		mesh_buffer(direct3d::device_t device, const mesh &mesh);
		~mesh_buffer();

		void activate(direct3d::context_t context);
		void draw(direct3d::context_t context);

		void activate_and_draw(direct3d::context_t context);

	private:
		void make_buffer(direct3d::device_t device, const std::vector<vertex> &vertices);
		void make_buffer(direct3d::device_t device, const std::vector<uint32_t> &indicies);

	private:
		buffer_t vertex_buffer;
		buffer_t index_buffer;

		uint32_t index_count{ 0 },
		         index_offset{ 0 },
		         vertex_size{ 0 },
		         vertex_offset{ 0 };
	};
}

