#include "constant_buffer.h"
#include "mesh_buffer.h"

using namespace planet_generator;

constant_buffer::constant_buffer(direct3d::device_t device, const transforms & data, shader_slot slot_) :
	slot(slot_)
{
	make_buffer(device, data);
}

constant_buffer::~constant_buffer() = default;

void constant_buffer::activate(direct3d::context_t context)
{
	// TODO: deal with other stages, for now only vertex

	ID3D11Buffer * const buffers[] = { buffer.get() };
	context->VSSetConstantBuffers(static_cast<uint32_t>(slot),
	                              1,
	                              buffers);
}

void constant_buffer::update(direct3d::context_t context, const transforms & data)
{
	D3D11_MAPPED_SUBRESOURCE gpu_buffer;

	HRESULT hr = context->Map(buffer.get(),
	                          NULL,
	                          D3D11_MAP_WRITE_DISCARD,
	                          NULL,
	                          &gpu_buffer);
	assert(hr == S_OK);

	transforms * gpu_data = reinterpret_cast<transforms *>(gpu_buffer.pData);
	gpu_data->data = data.data;

	context->Unmap(buffer.get(), NULL);
}

void constant_buffer::make_buffer(direct3d::device_t device, const transforms & data)
{
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.ByteWidth = sizeof(transforms);

	D3D11_SUBRESOURCE_DATA gpu_data{};
	gpu_data.pSysMem = static_cast<const void *>(&data);

	HRESULT hr = device->CreateBuffer(&bd,
	                                  &gpu_data,
	                                  buffer.put());
	assert(hr == S_OK);
}
