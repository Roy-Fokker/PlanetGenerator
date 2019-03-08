#include "renderer.h"

#include "direct3d.h"
#include "render_target.h"
#include "pipeline.h"
#include "mesh.h"

using namespace planet_generator;

namespace
{
	constexpr std::array<float, 4> clear_color{ 0.35f, 0.25f, 0.35f, 1.0f };
}


renderer::renderer(HWND hWnd)
{
	d3d = std::make_unique<direct3d>(hWnd);

	draw_target = std::make_unique<render_target>(d3d->as<direct3d::device_t>(),
												  d3d->as<direct3d::swap_chain_t>());
	draw_target->activate(d3d->as<direct3d::context_t>());
}

renderer::~renderer() = default;

void renderer::draw_frame()
{
	auto context = d3d->as<direct3d::context_t>();
	draw_target->clear(context, clear_color);


	d3d->present();
}

void renderer::resize_frame()
{
	draw_target.reset(nullptr);

	d3d->resize();

	draw_target = std::make_unique<render_target>(d3d->as<direct3d::device_t>(),
												  d3d->as<direct3d::swap_chain_t>());
	draw_target->activate(d3d->as<direct3d::context_t>());
}
