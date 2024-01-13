#include <fmt/core.h>

import input;
import window;
import application;

auto main() -> int
{
	using namespace std::string_view_literals;

	auto app = app_base::application();

	auto wnd = win32::window({ .width  = 800,
	                           .height = 600,
	                           .title  = L"First Triangle"sv });

	wnd.show();

	wnd.set_callback(app.on_keypress);
	wnd.set_callback(app.on_resize);
	wnd.set_callback(app.on_activate);

	while (wnd.handle() and app.should_continue())
	{
		// TODO: Create a clock class to get delta_time and total_time
		app.update(0.f, 0.f);

		wnd.process_messages();
	}

	return 0;
}