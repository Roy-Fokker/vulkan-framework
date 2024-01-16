import input;
import window;
import application;

import vfw;

auto read_file(const std::filesystem::path &filename) -> std::vector<uint32_t>
{
	auto file = std::ifstream(filename, std::ios::ate | std::ios::binary);

	if (not file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}

	auto file_size = file.tellg();
	auto buffer    = std::vector<uint32_t>(file_size);

	file.seekg(0);
	file.read(reinterpret_cast<char *>(buffer.data()), file_size);

	file.close();

	return buffer;
}

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

	auto rndr = vfw::renderer(wnd.handle());

	rndr.set_clear_color({ 0.4f, 0.4f, 0.2f, 1.f });

	auto vert_shader_bin = read_file("shaders/simple_shader.vert.spv");
	auto frag_shader_bin = read_file("shaders/simple_shader.frag.spv");

	while (wnd.handle() and app.should_continue())
	{
		// TODO: Create a clock class to get delta_time and total_time
		app.update(0.f, 0.f);

		rndr.draw_frame();

		wnd.process_messages();
	}

	return 0;
}