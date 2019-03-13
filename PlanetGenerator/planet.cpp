#include "planet.h"
#include "mesh_buffer.h"
#include <cmath>
#include <DirectXMath.h>

using namespace DirectX;
using namespace planet_generator;

namespace
{
	mesh make_cube(float side_length)
	{
		float l = side_length / 2.0f;
		float w = l;
		float h = l;

		return{
			// Vertex List
			{
				// Front
				{ { -l, -w, +h } }, //,{ 0.0f, 0.0f } },
				{ { +l, -w, +h } }, //,{ 1.0f, 0.0f } },
				{ { +l, +w, +h } }, //,{ 1.0f, 1.0f } },
				{ { -l, +w, +h } }, //,{ 0.0f, 1.0f } },

				// Bottom
				{ { -l, -w, -h } }, //,{ 0.0f, 0.0f } },
				{ { +l, -w, -h } }, //,{ 1.0f, 0.0f } },
				{ { +l, -w, +h } }, //,{ 1.0f, 1.0f } },
				{ { -l, -w, +h } }, //,{ 0.0f, 1.0f } },

				// Right
				{ { +l, -w, -h } }, //,{ 0.0f, 0.0f } },
				{ { +l, +w, -h } }, //,{ 1.0f, 0.0f } },
				{ { +l, +w, +h } }, //,{ 1.0f, 1.0f } },
				{ { +l, -w, +h } }, //,{ 0.0f, 1.0f } },

				// Left
				{ { -l, -w, -h } }, //,{ 0.0f, 0.0f } },
				{ { -l, -w, +h } }, //,{ 1.0f, 0.0f } },
				{ { -l, +w, +h } }, //,{ 1.0f, 1.0f } },
				{ { -l, +w, -h } }, //,{ 0.0f, 1.0f } },

				// Back
				{ { -l, -w, -h } }, //,{ 0.0f, 0.0f } },
				{ { -l, +w, -h } }, //,{ 1.0f, 0.0f } },
				{ { +l, +w, -h } }, //,{ 1.0f, 1.0f } },
				{ { +l, -w, -h } }, //,{ 0.0f, 1.0f } },

				// Top
				{ { -l, +w, -h } }, //,{ 0.0f, 0.0f } },
				{ { -l, +w, +h } }, //,{ 1.0f, 0.0f } },
				{ { +l, +w, +h } }, //,{ 1.0f, 1.0f } },
				{ { +l, +w, -h } }, //,{ 0.0f, 1.0f } },
			},


			// Index List
			{
				// Front
				0, 1, 2, 0, 2, 3,
				// Bottom
				4, 5, 6, 4, 6, 7,
				// Right
				8, 9, 10, 8, 10, 11,
				// Left
				12, 13, 14, 12, 14, 15,
				// Back
				16, 17, 18, 16, 18, 19,
				// Top
				20, 21, 22, 20, 22, 23,
			}
		};
	}

	void subdivide_mesh(mesh &mesh_obj, uint8_t subdivisions)
	{
		if (subdivisions < 1)
			return;

		if (subdivisions > 1)
			subdivide_mesh(mesh_obj, subdivisions - 1);

		mesh new_mesh{};

		size_t num_triangles = mesh_obj.indicies.size() / 3u;
		for (size_t i{ 0 }; i < num_triangles; i++)
		{
			vertex v0 = mesh_obj.verticies[mesh_obj.indicies[i * 3]],
				v1 = mesh_obj.verticies[mesh_obj.indicies[i * 3 + 1]],
				v2 = mesh_obj.verticies[mesh_obj.indicies[i * 3 + 2]];

			vertex m0, m1, m2;
			m0 = {
				{
					0.5f * (v0.position.x + v1.position.x),
					0.5f * (v0.position.y + v1.position.y),
					0.5f * (v0.position.z + v1.position.z)
				}/*,
				{
					0.5f * (v0.texCoord.x + v1.texCoord.x),
					0.5f * (v0.texCoord.y + v1.texCoord.y)
				}*/
			};

			m1 = {
				{
					0.5f * (v1.position.x + v2.position.x),
					0.5f * (v1.position.y + v2.position.y),
					0.5f * (v1.position.z + v2.position.z)
				}/*,
				{
					0.5f * (v1.texCoord.x + v2.texCoord.x),
					0.5f * (v1.texCoord.y + v2.texCoord.y)
				}*/
			};

			m2 = {
				{
					0.5f * (v0.position.x + v2.position.x),
					0.5f * (v0.position.y + v2.position.y),
					0.5f * (v0.position.z + v2.position.z)
				}/*,
				{
					0.5f * (v0.texCoord.x + v2.texCoord.x),
					0.5f * (v0.texCoord.y + v2.texCoord.y)
				}*/
			};

			new_mesh.verticies.insert(new_mesh.verticies.end(), {
				v0, v1, v2, m0, m1, m2
			});

			uint32_t n = static_cast<uint32_t>(i) * 6u;
			new_mesh.indicies.insert(new_mesh.indicies.end(), {
				n + 0, n + 3, n + 5,
				n + 3, n + 4, n + 5,
				n + 5, n + 4, n + 2,
				n + 3, n + 1, n + 4
			});
		}

		std::swap(mesh_obj.verticies, new_mesh.verticies);
		std::swap(mesh_obj.indicies, new_mesh.indicies);
	}

	void ensphere(mesh &mesh_obj, float radius)
	{
		for (size_t i{ 0 }; i < mesh_obj.verticies.size(); i++)
		{
			auto &v_p = mesh_obj.verticies[i];
			XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&v_p.position));
			XMVECTOR p = radius * n;

			XMStoreFloat3(&v_p.position, p);
		}
	}
}

mesh planet_generator::generate_sphere(float size, uint8_t subdivisions)
{
	float cube_length = (2.0f * size) / std::sqrt(3.0f);

	auto obj = make_cube(cube_length);
	subdivide_mesh(obj, subdivisions);
	ensphere(obj, size);

	return obj;
}

void planet_generator::layer_noise(noise_type type, mesh & mesh_obj)
{
}
