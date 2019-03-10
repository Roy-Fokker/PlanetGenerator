#pragma once

#include "renderer.h"
#include <cstdint>
#include <memory>

namespace planet_generator
{
	class window;
	class renderer;

	class application
	{
	public:
		application();
		~application();

		int run();

	private:
		bool keypress_callback(uintptr_t key_code, uintptr_t extension);
		bool resize_callback(uintptr_t wParam, uintptr_t lParam);

		void setup();
		void update();

	private:
		bool exit_application = false;
		std::unique_ptr<window> app_window = nullptr;
		std::unique_ptr<renderer> gfx_renderer = nullptr;

		renderer::handle mesh_id{};
		renderer::handle pipeline_id{};
	};
}
