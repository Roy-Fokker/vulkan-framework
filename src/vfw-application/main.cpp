import clock;
import input;
import window;
import application;

import vfw;

auto main() -> int
{
	using namespace std::string_view_literals;

	auto wnd = win32::window({ .width  = 800,
	                           .height = 600,
	                           .title  = L"Second Triangle"sv });

	wnd.show();

	auto rndr = vfw::renderer(wnd.handle());

	auto app = app_base::application(rndr);
	wnd.set_callback(app.on_keypress);
	wnd.set_callback([&](std::uint32_t width, std::uint32_t height) {
		rndr.window_resized(wnd.handle());
		return app.on_resize(width, height);
	});
	wnd.set_callback(app.on_activate);

	auto clk = app_clock::timer();

	while (wnd.handle() and app.should_continue())
	{
		clk.tick();

		auto dt = clk.get_delta<app_clock::s>();
		auto tt = clk.get_total<app_clock::s>();

		app.update(dt, tt);

		rndr.draw_frame();

		wnd.process_messages();
	}

	return 0;
}