#pragma once

#include <cstdint>
#include <string_view>
#include <memory>
#include <functional>
#include <Windows.h>

namespace planet_generator
{
	class window
	{
	public:
		struct size
		{
			uint16_t width;
			uint16_t height;
		};

		enum class style
		{
			normal,
			borderless,
			fullscreen
		};

		enum class message_name
		{
			resize,
			activate,
			keypress
		};
		static constexpr uint8_t max_message_names = 3;

		using callback_method = std::function<bool(uintptr_t, uintptr_t)>;

	public:
		window() = delete;
		window(std::wstring_view window_title, const size &window_size, const style window_style = style::normal, uint16_t window_icon = 0);
		~window();

		void on_message(message_name msg, const callback_method &callback);
		void process_messages();

		void show();
		void change_style(style window_style);
		void change_size(const size &window_size);

		HWND handle() const;

	private:
		struct window_implementation;
		std::unique_ptr<window_implementation> window_impl;
	};
}
