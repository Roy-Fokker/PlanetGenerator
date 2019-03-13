#pragma once

#include <cstdint>


namespace planet_generator
{
	struct mesh;

	mesh generate_sphere(float size, uint8_t subdivisions);

	enum class noise_type
	{
		simplex
	};

	void layer_noise(noise_type type, mesh &mesh_obj);

}
