#pragma once

#include <memory>
#include <vector>
#include <Windows.h>

namespace planet_generator
{
	class direct3d;
	class render_target;
	class pipeline;
	class mesh;

	class renderer
	{
	public:
		renderer() = delete;
		renderer(HWND hWnd);
		~renderer();
		
		void draw_frame();
		void resize_frame();

	private:
		std::unique_ptr<direct3d> d3d = nullptr;
		std::unique_ptr<render_target> draw_target = nullptr;

		std::unique_ptr<pipeline> temp_pipeline = nullptr;
		std::unique_ptr<mesh> temp_mesh = nullptr;
	};
}
