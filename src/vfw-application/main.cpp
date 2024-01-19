import input;
import window;
import application;

import vfw;

auto main() -> int
{
	using namespace std::string_view_literals;

	auto wnd = win32::window({ .width  = 800,
	                           .height = 600,
	                           .title  = L"First Triangle"sv });

	wnd.show();

	auto rndr = vfw::renderer(wnd.handle());

	auto app = app_base::application(rndr);
	wnd.set_callback(app.on_keypress);
	wnd.set_callback(app.on_resize);
	wnd.set_callback(app.on_activate);

	while (wnd.handle() and app.should_continue())
	{
		// TODO: Create a clock class to get delta_time and total_time
		app.update(0.f, 0.f);

		rndr.draw_frame();

		wnd.process_messages();
	}

	return 0;
}