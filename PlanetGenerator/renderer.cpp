#include "renderer.h"

#include "direct3d.h"
#include "render_target.h"
#include "pipeline_state.h"
#include "mesh_buffer.h"

#include <vector>
#include <fstream>

using namespace planet_generator;

// TODO: Move this whole block
namespace
{
	// TODO: Move somewhere else later
	using file_in_mem = std::vector<byte>;
	// TODO: Move somewhere else later
	file_in_mem read_binary_file(const std::wstring &fileName)
	{
		file_in_mem buffer;

		std::ifstream inFile(fileName, std::ios::in | std::ios::binary);
		inFile.unsetf(std::ios::skipws);

		if (!inFile.is_open())
		{
			throw std::runtime_error("Cannot open file");
		}

//		buffer.assign((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());
		
		inFile.seekg(0, std::ios::end);
		buffer.reserve(inFile.tellg());
		inFile.seekg(0, std::ios::beg);
		
		std::copy(std::istream_iterator<byte>(inFile),
				  std::istream_iterator<byte>(),
				  std::back_inserter(buffer));
		
		return buffer;
	}

	using vertex_array_t = std::vector<vertex>;
	using index_array_t = std::vector<uint32_t>;
	std::tuple<vertex_array_t, index_array_t> get_triangle_mesh(float base, float height, float delta)
	{
		float halfHeight = height / 2.0f;
		float halfBase = base / 2.0f;

		float x1 = -halfBase, y1 = -halfHeight,
			x3 = base * delta, y3 = halfHeight,
			x2 = halfBase, y2 = y1;

		return {
			// Vertex List
			{
				{ { x1, y1, +0.5f } },
				{ { x3, y3, +0.5f } },
				{ { x2, y2, +0.5f } },
			},

			// Index List
			{ 0, 1, 2 }
		};
	}
}

namespace
{
	constexpr std::array<float, 4> clear_color{ 0.35f, 0.25f, 0.35f, 1.0f };
}


renderer::renderer(HWND hWnd)
{
	d3d = std::make_unique<direct3d>(hWnd);
	auto device = d3d->get<direct3d::device_t>();

	draw_target = std::make_unique<render_target>(device,
												  d3d->get<direct3d::swap_chain_t>());
	draw_target->activate(d3d->get<direct3d::context_t>());

	auto vso = read_binary_file(L"position.vs.cso"),
	     pso = read_binary_file(L"green.ps.cso");
	temp_pipeline = std::make_unique<pipeline_state>(device,
													 pipeline_state::description{
														 pipeline_state::blend_mode::Opaque,
														 pipeline_state::depth_stencil_mode::ReadWrite,
														 pipeline_state::rasterizer_mode::CullAntiClockwise,
														 pipeline_state::sampler_mode::AnisotropicClamp,

														 D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
														 pipeline_state::input_layout_mode::position,
														 vso,
														 pso });

	auto[vertex_array, index_array] = get_triangle_mesh(1.0f, 1.0f, 0.0f);
	temp_mesh = std::make_unique<mesh_buffer>(device,
											  vertex_array,
											  index_array);
}

renderer::~renderer() = default;

void renderer::draw_frame()
{
	auto context = d3d->get<direct3d::context_t>();
	draw_target->clear(context, clear_color);

	temp_pipeline->activate(context);
	// set per frame shader constants 
	temp_mesh->activate(context);
	temp_mesh->draw(context);

	d3d->present();
}

void renderer::resize_frame()
{
	draw_target.reset(nullptr);

	d3d->resize();

	draw_target = std::make_unique<render_target>(d3d->get<direct3d::device_t>(),
												  d3d->get<direct3d::swap_chain_t>());
	draw_target->activate(d3d->get<direct3d::context_t>());
}
