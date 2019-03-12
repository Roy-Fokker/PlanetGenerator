#include "camera.h"

using namespace DirectX;
using namespace planet_generator;

camera::camera(const XMFLOAT3 & pos, const XMFLOAT3 & tgt, const XMFLOAT3 & up_vec)
{
	look_at(pos, tgt, up_vec);
}

camera::~camera() = default;

void camera::translate(float dolly, float pan, float crane)
{
	

	
}

void camera::rotate(float yaw, float pitch, float roll)
{

}

void camera::look_at(const XMFLOAT3 & pos, const XMFLOAT3 & tgt_, const XMFLOAT3 & up_vec)
{
	position = XMLoadFloat3(&pos);
	auto target = XMLoadFloat3(&tgt_),
		up = XMLoadFloat3(&up_vec);

	auto view_matrix = XMMatrixLookAtLH(position, target, up);
	orientation = XMQuaternionRotationMatrix(view_matrix);
}

const XMMATRIX camera::view() const
{
	auto translation = XMMatrixTranslationFromVector(-position);

	auto rotation = XMMatrixRotationQuaternion(orientation);

	return XMMatrixMultiply(translation, rotation);
}

XMMATRIX planet_generator::projection(float width, float height, float field_of_view, float near_plane, float far_plane)
{
	float aspect_ratio = width / height;

	float hFov = XMConvertToRadians(field_of_view);
	float vertical_fov = 2.0f * atanf(tanf(hFov / 2.0f) * aspect_ratio);

	auto proj = XMMatrixIdentity();
	proj = XMMatrixPerspectiveFovLH(vertical_fov, aspect_ratio, near_plane, far_plane);
	
	return proj;
	//XMFLOAT4X4 return_proj{};
	//XMStoreFloat4x4(&return_proj, proj);
	//return return_proj;
}
