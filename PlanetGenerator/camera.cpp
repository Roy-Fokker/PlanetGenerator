#include "camera.h"

using namespace DirectX;
using namespace planet_generator;

namespace
{
	static XMVECTOR forward_vec = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	static XMVECTOR left_vec = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	static XMVECTOR up_vec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMVECTOR rotate_vector(XMVECTOR &q, XMVECTOR &v)
	{
		auto vec = XMVectorSetW(v, 1.0f);
		auto conjugate = XMQuaternionConjugate(q);

		vec = XMQuaternionMultiply(q, vec);
		vec = XMQuaternionMultiply(vec, conjugate);

		return XMVector3Normalize(vec);
	}
}

camera::camera()
{
	look_at(XMFLOAT3{ 0.0f, 0.0f, -1.0f },   // From
	        XMFLOAT3{ 0.0f, 0.0f, 0.0f },    // At
	        XMFLOAT3{ 0.0f, 1.0f, 0.0f });   // Up
}

camera::camera(const XMFLOAT3 &pos, const XMFLOAT3 &tgt, const XMFLOAT3 &up)
{
	look_at(pos, tgt, up);
}

camera::~camera() = default;

void camera::translate(float dolly, float pan, float crane)
{
	auto forward = rotate_vector(orientation, forward_vec);
	auto left = rotate_vector(orientation, left_vec);
	auto up = rotate_vector(orientation, up_vec);

	//          Forward             Left           Up
	position = (forward * dolly) 
	         + (left * pan) 
	         + (up * crane) 
	         + position;
}

void camera::rotate(float yaw, float pitch, float roll)
{
	auto forward = rotate_vector(orientation, forward_vec);
	auto left = rotate_vector(orientation, left_vec);
	auto up = rotate_vector(orientation, up_vec);

	auto rot_x = XMQuaternionRotationAxis(left, pitch);
	auto rot_y = XMQuaternionRotationAxis(up, yaw);
	auto rot_z = XMQuaternionRotationAxis(forward, roll);

	orientation = XMQuaternionMultiply(rot_x, orientation);
	orientation = XMQuaternionMultiply(rot_y, orientation);
	orientation = XMQuaternionMultiply(rot_z, orientation);
}

void camera::look_at(const XMFLOAT3 & pos, const XMFLOAT3 & tgt_, const XMFLOAT3 & up_)
{
	position = XMLoadFloat3(&pos);
	auto target = XMLoadFloat3(&tgt_),
		up = XMLoadFloat3(&up_);

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
