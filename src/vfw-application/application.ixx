module;

#include <windows.h>

export module application;

import std;

import input;
import window;
import clock;

import vfw;

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
	public:
		application()  = delete;
		~application() = default;

		application(HWND hWnd)
		{
			setup_engine(hWnd);
		}

		void update(const std_clock::timer &clk, const win32::input &inpt)
		{
			dt = clk.get_delta<std_clock::s>();
			tt = clk.get_total<std_clock::s>();

			using enum win32::input_button;

			if (inpt.is_button_down(escape))
			{
				stop_app = true;
				std::println("⌚: {:>5.2f}s, ⏱️: {}ns, ⌨️: {:10.10}",
				             tt, dt, win32::to_string(escape));
			}
		}

		void render()
		{
		}

		[[nodiscard]] auto should_exit() const -> bool
		{
			return stop_app;
		}

		auto on_resize(std::uint32_t width, std::uint32_t height) -> bool
		{
			std::println("⌚: {:>5.2f}s, ⏱️: {}ns, Width: {:>5}, Height: {:>5}",
			             tt, dt, width, height);

			engine->window_resized(width, height);

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
		void setup_engine(HWND hWnd)
		{
			engine = std::make_unique<vfw::engine>(hWnd);

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

	private:
		bool stop_app = false;
		double dt     = 0.f;
		double tt     = 0.f;

		std::unique_ptr<vfw::engine> engine{};
	};
}