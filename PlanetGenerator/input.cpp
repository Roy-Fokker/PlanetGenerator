#include "input.h"

#include <cassert>

using namespace planet_generator;

namespace
{
	constexpr uint8_t RID_PAGE_DESKTOP = 0x01;
	constexpr uint8_t RID_USAGE_MOUSE = 0x02;
	constexpr uint8_t RID_USAGE_JOYSTICK = 0x04;
	constexpr uint8_t RID_USAGE_GAMEPAD = 0x05;
	constexpr uint8_t RID_USAGE_KEYBOARD = 0x06;

	constexpr RAWINPUTDEVICE raw_keyboard_device{
		RID_PAGE_DESKTOP,
		RID_USAGE_KEYBOARD,
		NULL,
		NULL
	};

	constexpr RAWINPUTDEVICE raw_mouse_device{
		RID_PAGE_DESKTOP,
		RID_USAGE_MOUSE,
		NULL,
		NULL
	};

	constexpr RAWINPUTDEVICE raw_gamepad_device{
		RID_PAGE_DESKTOP,
		RID_USAGE_GAMEPAD,
		NULL,
		NULL
	};

	constexpr RAWINPUTDEVICE raw_joystick_device{
		RID_PAGE_DESKTOP,
		RID_USAGE_JOYSTICK,
		NULL,
		NULL
	};
}

input::input(HWND hWnd_, const std::vector<input_device_type> &devices) :
	hWnd(hWnd_)
{
	buffer.fill({});
	keys_pressed.fill(false);
	axis_relative_positions.fill(0);
	axis_absolute_positions.fill(0);

	register_device(devices);
}

input::~input() = default;

void input::process_messages()
{
	uint32_t input_count = 0;
	MSG msg{};
	BOOL has_more_messages = TRUE;

	while (has_more_messages == TRUE && input_count < buffer.size())
	{
		has_more_messages = PeekMessage(&msg, hWnd, WM_INPUT, WM_INPUT, PM_NOYIELD | PM_REMOVE);

		if (has_more_messages == FALSE)
		{
			continue;
		}

		uint32_t raw_input_size = sizeof(RAWINPUT);
		uint32_t bytes_copied = GetRawInputData(reinterpret_cast<HRAWINPUT>(msg.lParam),
												 RID_INPUT,
												 &buffer[input_count],
												 &raw_input_size,
												 sizeof(RAWINPUTHEADER));
		assert(bytes_copied >= 0); // if asserted issues with copying data

		input_count++;
	}

	process_raw_input(input_count);
}

bool input::test_keypress(key_code key) const
{
	return keys_pressed[static_cast<uint8_t>(key)];
}

int16_t input::get_axis_relative(axis axis_) const
{
	return axis_relative_positions[static_cast<uint8_t>(axis_)];
}

int16_t input::get_axis_absolute(axis axis_) const
{
	return axis_absolute_positions[static_cast<uint8_t>(axis_)];
}

void input::register_device(const std::vector<input_device_type> &devices)
{
	std::vector<RAWINPUTDEVICE> rid{};

	for (auto device : devices)
	{
		switch (device)
		{
		case input_device_type::keyboard:
			rid.push_back(raw_keyboard_device);
			break;
		case input_device_type::mouse:
			rid.push_back(raw_mouse_device);
			break;
		case input_device_type::gamepad:
			rid.push_back(raw_gamepad_device);
			break;
		case input_device_type::joystick:
			rid.push_back(raw_joystick_device);
			break;
		}
	}

	for (auto &desc : rid)
	{
		desc.hwndTarget = hWnd;
	}

	auto result = RegisterRawInputDevices(rid.data(),
										  static_cast<uint32_t>(rid.size()),
										  sizeof(RAWINPUTDEVICE));
	assert(result == TRUE);
}

void input::process_raw_input(uint32_t count)
{
	axis_relative_positions.fill(0);

	for (uint32_t i = 0; i < count; i++)
	{
		const PRAWINPUT raw_input_data = &buffer.at(i);

		switch (raw_input_data->header.dwType)
		{
		case RIM_TYPEKEYBOARD:
			update_keys_pressed(raw_input_data->data.keyboard);
			break;
		case RIM_TYPEMOUSE:
			update_axis_data(raw_input_data->data.mouse);
			update_buttons_pressed(raw_input_data->data.mouse);
			break;
		default:
			assert(false); // TODO: implement GamePad and Joystick handling
		}
	}
}

void input::update_keys_pressed(const RAWKEYBOARD &data)
{
	uint16_t vKey = data.VKey;
	uint16_t sCode = data.MakeCode;
	uint16_t flags = data.Flags;
	bool kState = false;

	if (!(flags & RI_KEY_BREAK))
	{
		kState = true;
	}

	auto key = determine_key_code(vKey, sCode, flags);
	vKey = static_cast<uint16_t>(key);
	
	// TODO: Figure out what new key states [up, down, pressed]

	// Is this key a toggle key? if so change toggle state
	if (key == key_code::CapsLock
		|| key == key_code::NumLock
		|| key == key_code::Scroll)
	{
		kState = !keys_pressed[vKey];
	}

	// Update the Keyboard state array
	keys_pressed[vKey] = kState;

	// Update the Keyboard state where there are duplicate 
	// i.e Shift, Ctrl, and Alt
	switch (key)
	{
	case key_code::LShiftKey:
	case key_code::RShiftKey:
		keys_pressed[static_cast<uint16_t>(key_code::ShiftKey)] = kState;
		break;
	case key_code::LControlKey:
	case key_code::RControlKey:
		keys_pressed[static_cast<uint16_t>(key_code::ControlKey)] = kState;
		break;
	case key_code::LAltKey:
	case key_code::RAltKey:
		keys_pressed[static_cast<uint16_t>(key_code::AltKey)] = kState;
		break;
	}
}

input::key_code input::determine_key_code(uint16_t vKey, uint16_t sCode, uint16_t flags) const
{
	// return if the Key is out side of our enumeration range
	if (vKey > static_cast<uint16_t>(key_code::OemClear) ||
		vKey == static_cast<uint16_t>(key_code::None))
	{
		return key_code::None;
	}
	
	// figure out which key was press, in cases where there are duplicates (e.g numpad)
	const bool isE0 = ((flags & RI_KEY_E0) != 0);
	const bool isE1 = ((flags & RI_KEY_E1) != 0);

	switch (static_cast<key_code>(vKey))
	{
	case key_code::Pause:
		sCode = (isE1) ? 0x45 : MapVirtualKey(vKey, MAPVK_VK_TO_VSC);
		// What happens here????
		break;
	case key_code::ShiftKey:
		vKey = MapVirtualKey(sCode, MAPVK_VSC_TO_VK_EX);
		break;
	case key_code::ControlKey:
		vKey = static_cast<uint16_t>((isE0) ? key_code::RControlKey : key_code::LControlKey);
		break;
	case key_code::AltKey:
		vKey = static_cast<uint16_t>((isE0) ? key_code::RAltKey : key_code::LAltKey);
		break;
	case key_code::Enter:
		vKey = static_cast<uint16_t>((isE0) ? key_code::Separator : key_code::Enter);
		break;
	case key_code::Insert:
		vKey = static_cast<uint16_t>((!isE0) ? key_code::NumPad0 : key_code::Insert);
		break;
	case key_code::Delete:
		vKey = static_cast<uint16_t>((!isE0) ? key_code::Decimal : key_code::Delete);
		break;
	case key_code::Home:
		vKey = static_cast<uint16_t>((!isE0) ? key_code::NumPad7 : key_code::Home);
		break;
	case key_code::End:
		vKey = static_cast<uint16_t>((!isE0) ? key_code::NumPad1 : key_code::End);
		break;
	case key_code::Prior:
		vKey = static_cast<uint16_t>((!isE0) ? key_code::NumPad9 : key_code::Prior);
		break;
	case key_code::Next:
		vKey = static_cast<uint16_t>((!isE0) ? key_code::NumPad3 : key_code::Next);
		break;
	case key_code::Left:
		vKey = static_cast<uint16_t>((!isE0) ? key_code::NumPad4 : key_code::Left);
		break;
	case key_code::Right:
		vKey = static_cast<uint16_t>((!isE0) ? key_code::NumPad6 : key_code::Right);
		break;
	case key_code::Up:
		vKey = static_cast<uint16_t>((!isE0) ? key_code::NumPad8 : key_code::Up);
		break;
	case key_code::Down:
		vKey = static_cast<uint16_t>((!isE0) ? key_code::NumPad2 : key_code::Down);
		break;
	case key_code::Clear:
		vKey = static_cast<uint16_t>((!isE0) ? key_code::NumPad5 : key_code::Clear);
		break;
	}

	return static_cast<key_code>(vKey);
}

void input::update_axis_data(const RAWMOUSE &data)
{
	int32_t xPos = data.lLastX;
	int32_t yPos = data.lLastY;
	
	int16_t btnData = data.usButtonData;
	
	int16_t flags = data.usFlags;
	int16_t btnFlags = data.usButtonFlags;

	if (flags & MOUSE_MOVE_ABSOLUTE)
	{
		// Update Axis values
		axis_relative_positions[static_cast<int8_t>(axis::X)] = xPos;
		axis_relative_positions[static_cast<int8_t>(axis::Y)] = yPos;
		// Update Absolute values for axis
		axis_absolute_positions[static_cast<int8_t>(axis::X)] = xPos;
		axis_absolute_positions[static_cast<int8_t>(axis::Y)] = yPos;
	}
	else
	{
		// Update Axis values
		axis_relative_positions[static_cast<int8_t>(axis::X)] += xPos;
		axis_relative_positions[static_cast<int8_t>(axis::Y)] += yPos;
		// Update Absolute values for axis
		axis_absolute_positions[static_cast<int8_t>(axis::X)] += xPos;
		axis_absolute_positions[static_cast<int8_t>(axis::Y)] += yPos;
	}

	// Vertical wheel data
	if (btnFlags & RI_MOUSE_WHEEL)
	{
		axis_relative_positions[static_cast<int8_t>(axis::RX)] += btnData;
		axis_absolute_positions[static_cast<int8_t>(axis::RX)] += btnData;
	}

	// Horizontal wheel data 
	// NOTE: untested code. don't have device that supports this
	if (btnFlags & RI_MOUSE_HWHEEL)
	{
		axis_relative_positions[static_cast<int8_t>(axis::RY)] += btnData;
		axis_absolute_positions[static_cast<int8_t>(axis::RY)] += btnData;
	}
}

void input::update_buttons_pressed(const RAWMOUSE &data)
{
	int16_t btnFlags = data.usButtonFlags;

	key_code btn = determine_key_code(btnFlags);
	uint16_t vBtn = static_cast<uint16_t>(btn);

	// What is the button state?
	bool btnState{ false };
	if ((btnFlags & RI_MOUSE_BUTTON_1_DOWN)
		|| (btnFlags & RI_MOUSE_BUTTON_2_DOWN)
		|| (btnFlags & RI_MOUSE_BUTTON_3_DOWN)
		|| (btnFlags & RI_MOUSE_BUTTON_4_DOWN)
		|| (btnFlags & RI_MOUSE_BUTTON_5_DOWN))
	{
		btnState = true;
	}

	// TODO: Figure out what new key states [up, down, pressed]

	keys_pressed[vBtn] = btnState;
}

input::key_code input::determine_key_code(uint16_t btnFlags) const
{
	key_code btn = key_code::None;
	// Which button was pressed?
	if ((btnFlags & RI_MOUSE_BUTTON_1_DOWN) || (btnFlags & RI_MOUSE_BUTTON_1_UP))
	{
		btn = key_code::LButton; // MK_LBUTTON;
	}
	else if ((btnFlags & RI_MOUSE_BUTTON_2_DOWN) || (btnFlags & RI_MOUSE_BUTTON_2_UP))
	{
		btn = key_code::RButton; // MK_RBUTTON;
	}
	else if ((btnFlags & RI_MOUSE_BUTTON_3_DOWN) || (btnFlags & RI_MOUSE_BUTTON_3_UP))
	{
		btn = key_code::MButton; // MK_MBUTTON;
	}
	else if ((btnFlags & RI_MOUSE_BUTTON_4_DOWN) || (btnFlags & RI_MOUSE_BUTTON_4_UP))
	{
		btn = key_code::XButton1; // MK_XBUTTON1;
	}
	else if ((btnFlags & RI_MOUSE_BUTTON_5_DOWN) || (btnFlags & RI_MOUSE_BUTTON_5_UP))
	{
		btn = key_code::XButton2; // MK_XBUTTON2;
	}

	return btn;
}
