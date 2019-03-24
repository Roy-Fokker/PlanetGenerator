#include "mesh_buffer.h"

using namespace planet_generator;

mesh_buffer::mesh_buffer(direct3d::device_t device, const std::vector<vertex>& vertices, const std::vector<uint32_t>& indicies)
{
	make_buffer(device, vertices);
	make_buffer(device, indicies);
}

mesh_buffer::mesh_buffer(direct3d::device_t device, const mesh & mesh)
{
	make_buffer(device, mesh.verticies);
	make_buffer(device, mesh.indicies);
}

mesh_buffer::~mesh_buffer() = default;

void mesh_buffer::activate(direct3d::context_t context)
{
	ID3D11Buffer * const vert_buffers[] = { vertex_buffer.get() };
	context->IASetVertexBuffers(0,
	                            1,
	                            vert_buffers,
	                            &vertex_size,
	                            &vertex_offset);

	context->IASetIndexBuffer(index_buffer.get(),
	                          DXGI_FORMAT_R32_UINT,
	                          index_offset);
}

void mesh_buffer::draw(direct3d::context_t context)
{
	context->DrawIndexed(index_count,
	                     0,
	                     0);
}

void mesh_buffer::activate_and_draw(direct3d::context_t context)
{
	activate(context);
	draw(context);
}

void mesh_buffer::make_buffer(direct3d::device_t device, const std::vector<vertex>& vertices)
{
	vertex_size = sizeof(vertices.back());

	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = NULL;
	bd.ByteWidth = vertex_size * static_cast<uint32_t>(vertices.size());

	D3D11_SUBRESOURCE_DATA vertex_data{};
	vertex_data.pSysMem = reinterpret_cast<const void *>(vertices.data());

	auto hr = device->CreateBuffer(&bd,
	                               &vertex_data,
	                               vertex_buffer.put());
	assert(hr == S_OK);
}

void mesh_buffer::make_buffer(direct3d::device_t device, const std::vector<uint32_t>& indicies)
{
	index_count = static_cast<uint32_t>(indicies.size());

	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = NULL;
	bd.ByteWidth = sizeof(uint32_t) * index_count;

	D3D11_SUBRESOURCE_DATA index_data{};
	index_data.pSysMem = reinterpret_cast<const void *>(indicies.data());

	auto hr = device->CreateBuffer(&bd,
	                               &index_data,
	                               index_buffer.put());
	assert(hr == S_OK);
}
