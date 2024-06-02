import std;

import clock;
import input;
import window;
import application;

auto main() -> int
{
	using namespace std::string_view_literals;
	using namespace win32;

	std::println("Running in: {}", std::filesystem::current_path().generic_string());

	auto wnd = win32::window({ .width  = 800,
	                           .height = 600,
	                           .title  = L"Vulkan Framework Application"sv });

	auto inpt = input(wnd.handle(), { input_device::keyboard, input_device::mouse });
	auto app  = app_base::application(wnd.handle());

	// Callbacks provided by window class.
	wnd.set_callback([&]([[maybe_unused]] input_button button,
	                     [[maybe_unused]] std::uint16_t scan_code,
	                     [[maybe_unused]] bool isKeyDown,
	                     [[maybe_unused]] std::uint16_t repeat_count) {
		return false;
	});
	wnd.set_callback([&](std::uint32_t width, std::uint32_t height) {
		return app.on_resize(width, height);
	});
	wnd.set_callback([&](window::active_state is_active, bool minimized) {
		return app.on_activate(is_active, minimized);
	});

	auto clk = std_clock::timer();

	wnd.show();

	while (wnd.handle() and not app.should_exit())
	{
		wnd.process_messages();
		inpt.process_messages();
		clk.tick();

		app.update(clk, inpt);
		app.render();
	}

	std::println("Loop run time: {:>5.2f}s", clk.get_total<std_clock::s>());

	return EXIT_SUCCESS;
}