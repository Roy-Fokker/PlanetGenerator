#pragma once


#include "window.h"

#include <cstdint>
#include <memory>


namespace planet_generator
{
	class window;

	class application
	{
	public:
		application();

		int run();

	private:
		bool keypress_callback(uintptr_t key_code, uintptr_t extension);
		bool resize_callback(uintptr_t wParam, uintptr_t lParam);

	private:
		bool exit_application = false;
		std::unique_ptr<window> app_window = nullptr;
		//std::unique_ptr<graphics_renderer> gfx_renderer = nullptr;
	};
}
