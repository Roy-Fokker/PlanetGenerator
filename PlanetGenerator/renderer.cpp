#include "renderer.h"

#include "direct3d.h"
#include "render_target.h"
#include "pipeline_state.h"
#include "mesh_buffer.h"
#include "constant_buffer.h"

using namespace planet_generator;

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
}

renderer::~renderer() = default;

renderer::handle renderer::add_mesh(const mesh & mesh_data)
{
	meshes.push_back(std::make_unique<mesh_buffer>(d3d->get<direct3d::device_t>(), 
												   mesh_data));

	return { object_type::mesh, static_cast<uint32_t>(meshes.size()) };
}

renderer::handle renderer::add_pipeline_state(const pipeline_description & description)
{
	pipeline_states.push_back(std::make_unique<pipeline_state>(d3d->get<direct3d::device_t>(),
															   description));

	return { object_type::pipeline, static_cast<uint32_t>(pipeline_states.size()) };;
}

renderer::handle renderer::add_transform(const transforms & transform, uint16_t slot)
{
	constant_buffers.push_back(std::make_unique<constant_buffer>(d3d->get<direct3d::device_t>(),
																transform,
																slot));

	return { object_type::transform, static_cast<uint32_t>(constant_buffers.size()) };
}

void renderer::update_transform(uint32_t id, const transforms & transform)
{
	constant_buffers.at(id)->update(d3d->get<direct3d::context_t>(), transform);
}

void renderer::add_to_draw_queue(handle handle_)
{
	draw_queue.push(handle_);
}

void renderer::draw_frame()
{
	auto context = d3d->get<direct3d::context_t>();
	draw_target->clear(context, clear_color);

	while (not draw_queue.empty())
	{
		auto [obj_type, id] = draw_queue.front();
		draw_queue.pop();

		activate(context, obj_type, --id);
	}

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

void planet_generator::renderer::activate(winrt::com_ptr<ID3D11DeviceContext>& context, object_type obj_type, const uint32_t &id)
{
	switch (obj_type)
	{
	case object_type::mesh:
		meshes.at(id)->activate_and_draw(context);
		break;
	case object_type::pipeline:
		pipeline_states.at(id)->activate(context);
		break;
	case object_type::transform:
		constant_buffers.at(id)->activate(context);
		break;
	}
}
