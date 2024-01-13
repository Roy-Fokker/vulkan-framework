#include <fmt/core.h>

import input;
import window;

auto main() -> int
{
	using namespace std::string_view_literals;

	auto wnd = win32::window({ .width  = 800,
	                           .height = 600,
	                           .title  = L"First Triangle"sv });

	wnd.show();

	// wnd.set_callback(game.on_keypress);
	// wnd.set_callback(game.on_resize);
	// wnd.set_callback(game.on_activate);

	while (wnd.handle())
	{
		wnd.process_messages();
	}

	return 0;
}