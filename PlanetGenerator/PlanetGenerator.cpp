#include "PlanetGenerator.h"

#include "window/window.h"
#include "input.h"
#include "graphics/mesh_buffer.h"
#include "graphics/pipeline_state.h"
#include "graphics/constant_buffer.h"
#include "graphics/material.h"
#include "camera.h"

#include "planet.h"

#include <vector>
#include <fstream>
#include <DirectXMath.h>

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
}


application::application()
{
	constexpr uint16_t height = 600;
	constexpr uint16_t width = height * 16 / 10;

	app_window = std::make_unique<window>(L"Planet Generator",
										  window::size{ width, height });

	app_window->on_message(window::message_name::resize,
	                       [&](uintptr_t wParam, uintptr_t lParam) -> bool
	{
		return resize_callback(wParam, lParam);
	});


	app_input = std::make_unique<input>(app_window->handle(),
										std::vector{ input::input_device_type::keyboard, input::input_device_type::mouse });

	gfx_renderer = std::make_unique<renderer>(app_window->handle());

	camera_view = std::make_unique<camera>();
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

		app_input->process_messages();
		app_window->process_messages();
	}

	return 0;
}

void application::update_input()
{
	// TODO: Input Context
	using key = input::key_code;

	if (app_input->test_keypress(key::Escape))
	{
		exit_application = true;
	}

	float move_by = 0.005f;

	if (app_input->test_keypress(key::W))
		camera_view->translate(move_by, 0.f, 0.f);
	else if (app_input->test_keypress(key::S))
		camera_view->translate(-move_by, 0.f, 0.f);
	
	if (app_input->test_keypress(key::A))
		camera_view->translate(0.f, move_by, 0.f);
	else if(app_input->test_keypress(key::D))
		camera_view->translate(0.f, -move_by, 0.f);
	
	if (app_input->test_keypress(key::Q))
		camera_view->translate(0.f, 0.f, move_by);
	else if (app_input->test_keypress(key::E))
		camera_view->translate(0.f, 0.f, -move_by);


	if (app_input->test_keypress(key::J))
		camera_view->rotate(move_by, 0.f, 0.f);
	else if (app_input->test_keypress(key::L))
		camera_view->rotate(-move_by, 0.f, 0.f);
		
	if (app_input->test_keypress(key::I))
		camera_view->rotate(0.f, move_by, 0.f);
	else if (app_input->test_keypress(key::K))
		camera_view->rotate(0.f, -move_by, 0.f);
		
	if (app_input->test_keypress(key::U))
		camera_view->rotate(0.f, 0.f, move_by);
	else if (app_input->test_keypress(key::O))
		camera_view->rotate(0.f, 0.f, -move_by);
	


	
}

bool application::resize_callback(uintptr_t wParam, uintptr_t lParam)
{
	gfx_renderer->resize_frame();
	return false;
}

void application::setup()
{
	/* Pipeline State setup */ {
		pipeline_id = gfx_renderer->add_pipeline_state(
			pipeline_description{
				pipeline_state::blend_mode::Opaque,
				pipeline_state::depth_stencil_mode::ReadWrite,
				pipeline_state::rasterizer_mode::Wireframe,
				pipeline_state::sampler_mode::AnisotropicClamp,

				D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
				});
	}

	/* Material setup */ {
		auto vso = read_binary_file(L"position.vs.cso"),
		     pso = read_binary_file(L"green.ps.cso");

		material_id = gfx_renderer->add_material(
			material_description{
				{ material::input_layout_mode::position },
				vso,
				pso
			});
	}

	/* Mesh setup */ {
		auto planet = generate_sphere(1.0f, 6);
		layer_noise(noise_type::simplex, planet);
		mesh_id = gfx_renderer->add_mesh(planet);
	}

	/* Mesh transform setup */ {
		constexpr auto angle = DirectX::XMConvertToRadians(45.0f);
		auto tdata = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
		tdata *= DirectX::XMMatrixRotationRollPitchYaw(angle, 0.0f, angle);
		transform_id = gfx_renderer->add_transform(transforms{ DirectX::XMMatrixTranspose(tdata) },
		                                           shader_slot::transform);
	}

	/* Projection Matrix setup */ {
		RECT rect{};
		GetClientRect(app_window->handle(), &rect);
		auto width = static_cast<uint16_t>(rect.right - rect.left);
		auto height = static_cast<uint16_t>(rect.bottom - rect.top);
		auto tdata = projection(width, height, 60.0f, 0.1f, 1000.0f);
		projection_id = gfx_renderer->add_transform(transforms{ DirectX::XMMatrixTranspose(tdata) },
		                                            shader_slot::projection);
	}

	/* View Matrix Setup */ {
		camera_view->look_at(DirectX::XMFLOAT3{ 0.0f, 0.0f, -2.0f },
		                     DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		                     DirectX::XMFLOAT3{ 0.0f, 1.0f, 0.0f });
		auto tdata = camera_view->view();
		view_id = gfx_renderer->add_transform(transforms{ DirectX::XMMatrixTranspose(tdata) },
		                                      shader_slot::view);
	}
}

void application::update()
{
	/* Update Input */
	update_input();

	/* Update the Camera location */ {
		auto tdata = camera_view->view();
		gfx_renderer->update_transform(view_id, transforms{ DirectX::XMMatrixTranspose(tdata) });
	}

	/* Rotate the planet mesh */ {
		static float r = 0.0f;
		auto angle = DirectX::XMConvertToRadians(r);
		auto tdata = DirectX::XMMatrixRotationRollPitchYaw(0.0f, angle, 0.0f);
		gfx_renderer->update_transform(transform_id, transforms{ DirectX::XMMatrixTranspose(tdata) });
		r += 0.01f;
		if (r > 360.0f) r = 0.0f;
	}

	/* Fill the Draw Queue */
	gfx_renderer->add_to_draw_queue(view_id);
	gfx_renderer->add_to_draw_queue(projection_id);
	gfx_renderer->add_to_draw_queue(pipeline_id);
	gfx_renderer->add_to_draw_queue(transform_id);
	gfx_renderer->add_to_draw_queue(material_id);
	gfx_renderer->add_to_draw_queue(mesh_id);
}
