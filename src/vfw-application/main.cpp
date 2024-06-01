import std;

import clock;
import input;
import window;
import application;

import vfw;

auto main() -> int
{
	using namespace std::string_view_literals;

	std::println("Running in: {}", std::filesystem::current_path().generic_string());

	auto wnd = win32::window({ .width  = 800,
	                           .height = 600,
	                           .title  = L"Second Triangle"sv });

	wnd.show();

	auto rndr = 0u;
	auto app = app_base::application(rndr);
	
	wnd.set_callback([&](std::uint32_t width, std::uint32_t height) {
		return app.on_resize(width, height);
	});
	wnd.set_callback(app.on_activate);

	auto clk = std_clock::timer();
	auto inpt = win32::input(wnd.handle(), { win32::input_device::keyboard, win32::input_device::mouse });

	while (wnd.handle() and app.should_continue())
	{
		clk.tick();

		app.update(clk, inpt);

		inpt.process_messages();
		wnd.process_messages();
	}

	std::println("Loop run time: {:>5.2f}s", clk.get_total<std_clock::s>());

	return 0;
}