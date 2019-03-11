#pragma once

#include <Windows.h>
#include <winrt/base.h>
#include <d3d11_1.h>
#include <memory>
#include <vector>
#include <tuple>
#include <queue>

namespace planet_generator
{
	class direct3d;
	class render_target;
	class pipeline_state;
	struct pipeline_description;
	class mesh_buffer;
	struct mesh;
	class constant_buffer;
	struct transforms;

	class renderer
	{
	public:
		enum class object_type
		{
			transform,
			pipeline,
			mesh
		};

		struct handle
		{
			object_type type;
			uint32_t id;
		};

	private:
		using mesh_buffer_ptr = std::unique_ptr<mesh_buffer>;
		using pipeline_state_ptr = std::unique_ptr<pipeline_state>;
		using constant_buffer_ptr = std::unique_ptr<constant_buffer>;
		
	public:
		renderer() = delete;
		renderer(HWND hWnd);
		~renderer();
		
		[[nodiscard]]
		handle add_mesh(const mesh &mesh_data);
		[[nodiscard]]
		handle add_pipeline_state(const pipeline_description &description);
		[[nodiscard]]
		handle add_transform(const transforms &transform, uint16_t slot);
		void update_transform(uint32_t id, const transforms &transform);

		void add_to_draw_queue(handle handle_);

		void draw_frame();
		void resize_frame();

	private:
		void activate(winrt::com_ptr<ID3D11DeviceContext> &context, object_type obj_type, const uint32_t &id);

	private:
		std::unique_ptr<direct3d> d3d = nullptr;
		std::unique_ptr<render_target> draw_target = nullptr;

		std::vector<mesh_buffer_ptr> meshes;
		std::vector<pipeline_state_ptr> pipeline_states;
		std::vector<constant_buffer_ptr> constant_buffers;

		std::queue<handle> draw_queue;
	};

	
}
