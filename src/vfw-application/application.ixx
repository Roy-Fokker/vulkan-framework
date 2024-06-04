module;

#include <windows.h>

export module application;

import std;

import input;
import window;
import clock;

import vfw;

using namespace std::string_view_literals;

export namespace app_base
{
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

	class application
	{
	private:
		std::array<uint16_t, 2> window_size = { 800, 600 };
		std::wstring_view window_title      = L"Vulkan Framework Application"sv;
		bool stop_app                       = false;
		double dt                           = 0.f;
		double tt                           = 0.f;

	public:
		~application() = default;

		application()
		{
			setup_window_and_input();

			setup_engine();
		}

		auto run() -> bool
		{
			clock.reset();
			window.show();

			while (window.handle() and not stop_app)
			{
				window.process_messages();
				input.process_messages();
				clock.tick();

				update();
				render();
			}

			return EXIT_SUCCESS;
		}

	private:
		void setup_window_and_input()
		{
			using namespace win32;

			window = win32::window({ .width  = window_size[0],
			                         .height = window_size[1],
			                         .title  = window_title });

			input = win32::input(window.handle(),
			                     { win32::input_device::keyboard, win32::input_device::mouse });

			clock = std_clock::timer();

			// Callbacks provided by window class.
			window.set_callback([&]([[maybe_unused]] input_button button,
			                        [[maybe_unused]] std::uint16_t scan_code,
			                        [[maybe_unused]] bool isKeyDown,
			                        [[maybe_unused]] std::uint16_t repeat_count) {
				return false;
			});
			window.set_callback([&](std::uint16_t width, std::uint16_t height) {
				return on_resize(width, height);
			});
			window.set_callback([&](window::active_state is_active, bool minimized) {
				return on_activate(is_active, minimized);
			});
		}

		void setup_engine()
		{
			renderer = std::make_unique<vfw::renderer>(window.handle());

			setup_pipeline();
			setup_model();
			add_draw_cmds();
		}

		void setup_pipeline()
		{
			auto vert_shader_bin = read_file("shaders/basic_pc_shader.vert.spv");
			auto frag_shader_bin = read_file("shaders/basic_pc_shader.frag.spv");
		}

		void setup_model()
		{
		}

		void add_draw_cmds()
		{
		}

		void update()
		{
			dt = clock.get_delta<std_clock::s>();
			tt = clock.get_total<std_clock::s>();

			using enum win32::input_button;

			if (input.is_button_down(escape))
			{
				stop_app = true;
				std::println("⌚: {:>5.2f}s, ⏱️: {}ns, ⌨️: {:10.10}",
				             tt, dt, win32::to_string(escape));
			}
		}

		void render()
		{
			renderer->draw();
		}

		auto on_resize(std::uint16_t width, std::uint16_t height) -> bool
		{
			std::println("⌚: {:>5.2f}s, ⏱️: {}ns, Width: {:>5}, Height: {:>5}",
			             tt, dt, width, height);

			window_size = { width, height };

			if (renderer)
			{
				renderer->window_resized(width, height);
			}

			return true;
		}

		auto on_activate(win32::window::active_state is_active, bool minimized) -> bool
		{
			auto state = (is_active == win32::window::active_state::active) ? true : false;

			std::println("⌚: {:>5.2f}s, ⏱️: {}s, Active: {:^5}, Minimized: {:^5}",
			             tt, dt, state, minimized);

			return true;
		}

	private:
		win32::window window{};
		win32::input input{};
		std_clock::timer clock{};

		std::unique_ptr<vfw::renderer> renderer{};
	};
}