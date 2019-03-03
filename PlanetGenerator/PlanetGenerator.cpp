#include "PlanetGenerator.h"

using namespace planet_generator;

application::application()
{
	constexpr uint16_t height = 600;
	constexpr uint16_t width = height * 16 / 10;

	app_window = std::make_unique<window>(L"Planet Generator",
										  window::size{ width, height });

	app_window->on_message(window::message_name::keypress,
						   [&](uintptr_t key_code, uintptr_t extension) -> bool
	{
		return keypress_callback(key_code, extension);
	});

	app_window->on_message(window::message_name::resize,
						   [&](uintptr_t wParam, uintptr_t lParam) -> bool
	{
		return resize_callback(wParam, lParam);
	});
}


int application::run()
{
	app_window->show();

	while (app_window->handle() and (not exit_application))
	{
		app_window->process_messages();
	}

	return 0;
}

bool application::keypress_callback(uintptr_t key_code, uintptr_t extension)
{
	switch (key_code)
	{
	case VK_ESCAPE:
		exit_application = true;
		break;
	}
	return true;
}

bool application::resize_callback(uintptr_t wParam, uintptr_t lParam)
{
	return false;
}
