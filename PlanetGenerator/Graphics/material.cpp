#include "material.h"
#include <cassert>

using namespace planet_generator;

namespace
{
	using element_desc = std::vector<D3D11_INPUT_ELEMENT_DESC>;
	constexpr D3D11_INPUT_ELEMENT_DESC position = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	constexpr D3D11_INPUT_ELEMENT_DESC normal   = { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	constexpr D3D11_INPUT_ELEMENT_DESC texcoord = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
}

material::material(direct3d::device_t device, const description &mat_desc)
{
	make_input_layout(device, mat_desc.input_layout, mat_desc.vertex_shader_file);
	make_vertex_shader(device, mat_desc.vertex_shader_file);
	make_pixel_shader(device, mat_desc.pixel_shader_file);
}

material::~material() = default;

void material::activate(direct3d::context_t context)
{
	context->IASetInputLayout(input_layout.get());

	context->VSSetShader(vertex_shader.get(), nullptr, 0);
	context->PSSetShader(pixel_shader.get(), nullptr, 0);
}

void material::make_input_layout(direct3d::device_t device, const std::vector<material::input_layout_mode> input_layout_list, const std::vector<byte>& vso)
{
	element_desc elements;
	for (auto layout_type : input_layout_list)
	{
		switch (layout_type)
		{
		case input_layout_mode::position:
			elements.push_back(position);
			break;
		case input_layout_mode::normal:
			elements.push_back(normal);
			break;
		case input_layout_mode::texcoord:
			elements.push_back(texcoord);
			break;
		default:
			assert(false); // Unimplemented Enum value
			break;
		}
	}

	auto hr = device->CreateInputLayout(elements.data(),
	                                    static_cast<uint32_t>(elements.size()),
	                                    vso.data(),
	                                    static_cast<uint32_t>(vso.size()),
	                                    input_layout.put());
	assert(hr == S_OK);
}

void material::make_vertex_shader(direct3d::device_t device, const std::vector<byte>& vso)
{
	auto hr = device->CreateVertexShader(vso.data(),
	                                     vso.size(),
	                                     NULL,
	                                     vertex_shader.put());
	assert(hr == S_OK);
}

void material::make_pixel_shader(direct3d::device_t device, const std::vector<byte>& pso)
{
	auto hr = device->CreatePixelShader(pso.data(),
	                                    pso.size(),
	                                    NULL,
	                                    pixel_shader.put());
	assert(hr == S_OK);
}
