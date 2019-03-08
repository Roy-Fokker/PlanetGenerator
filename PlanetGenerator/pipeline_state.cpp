#include "pipeline_state.h"

using namespace planet_generator;

pipeline_state::pipeline_state(direct3d::device_t device, const description & state_desc)
{
}

pipeline_state::~pipeline_state() = default;

void pipeline_state::activate(direct3d::context_t context)
{
}

void pipeline_state::make_blend_state(direct3d::device_t device, blend_mode blend)
{
}

void pipeline_state::make_depth_stencil_state(direct3d::device_t device, depth_stencil_mode depth_stencil)
{
}

void pipeline_state::make_rasterizer_state(direct3d::device_t device, rasterizer_mode rasterizer)
{
}

void pipeline_state::make_sampler_state(direct3d::device_t device, sampler_mode sampler)
{
}

void pipeline_state::make_input_layout(direct3d::device_t device, input_layout_mode input_layout, const std::vector<byte>& vso)
{
}

void pipeline_state::make_vertex_shader(direct3d::device_t device, const std::vector<byte>& vso)
{
}

void pipeline_state::make_pixel_shader(direct3d::device_t device, const std::vector<byte>& pso)
{
}
