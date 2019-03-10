#include "PlanetGenerator.h"

#include "window.h"
#include "mesh_buffer.h"
#include "pipeline_state.h"

#include <vector>
#include <fstream>

using namespace planet_generator;

namespace
{
	mesh get_triangle_mesh(float base, float height, float delta)
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
}


application::application()
{
	constexpr uint16_t height = 600;
	constexpr uint16_t width = height * 16 / 10;

	app_window = std::make_unique<window>(L"Planet Generator",
										  window::size{ width, height });

	app_window->on_message(window::message_name::keypress,
						   [&](uintptr_t key_code, uintptr_t extension) -> bool
	{
		return keypress_callback(key_code, extension);
	});

	app_window->on_message(window::message_name::resize,
						   [&](uintptr_t wParam, uintptr_t lParam) -> bool
	{
		return resize_callback(wParam, lParam);
	});

	gfx_renderer = std::make_unique<renderer>(app_window->handle());
}

application::~application() = default;

int application::run()
{
	setup();

	app_window->show();

	while (app_window->handle() and (not exit_application))
	{
		update();
		gfx_renderer->draw_frame();

		app_window->process_messages();
	}

	return 0;
}

bool application::keypress_callback(uintptr_t key_code, uintptr_t extension)
{
	switch (key_code)
	{
	case VK_ESCAPE:
		exit_application = true;
		break;
	}
	return true;
}

bool application::resize_callback(uintptr_t wParam, uintptr_t lParam)
{
	gfx_renderer->resize_frame();
	return false;
}

void application::setup()
{
	mesh_id = gfx_renderer->add_mesh(get_triangle_mesh(1.0f, 1.0f, 0.0f));

	auto vso = read_binary_file(L"position.vs.cso"),
	     pso = read_binary_file(L"green.ps.cso");

	pipeline_id = gfx_renderer->add_pipeline_state(
		pipeline_description{
			pipeline_state::blend_mode::Opaque,
			pipeline_state::depth_stencil_mode::ReadWrite,
			pipeline_state::rasterizer_mode::CullAntiClockwise,
			pipeline_state::sampler_mode::AnisotropicClamp,
			
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			pipeline_state::input_layout_mode::position,
			vso,
			pso });
}

void application::update()
{
	gfx_renderer->add_to_draw_queue(pipeline_id);
	gfx_renderer->add_to_draw_queue(mesh_id);
}
