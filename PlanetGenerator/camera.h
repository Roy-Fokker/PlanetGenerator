#pragma once

#include <DirectXMath.h>

namespace planet_generator
{
	[[nodiscard]]
	DirectX::XMMATRIX projection(float width, float height, float field_of_view, float near_plane, float far_plane);
	
	// TODO: Make this generic, so that it can be used for any mesh
	class camera
	{
	public:
		camera();
		camera(const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up);
		~camera();

		void translate(float dolly, float pan, float crane);
		void rotate(float yaw, float pitch, float roll);
		
		void look_at(const DirectX::XMFLOAT3 &position, const DirectX::XMFLOAT3 &target, const DirectX::XMFLOAT3 &up);

		[[nodiscard]]
		const DirectX::XMMATRIX view() const;

	private:
		DirectX::XMVECTOR position{};
		DirectX::XMVECTOR orientation{};
	};
}