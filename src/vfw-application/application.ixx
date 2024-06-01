module;

export module application;

import std;

import input;
import window;
import clock;

import vfw;

export namespace app_base
{
	struct push_constant
	{
		alignas(16) glm::vec2 pos;
	};

	class application
	{
	public:
		application() = delete;
		explicit application(std::uint32_t &rndr)
		{
			setup_renderer(rndr);
		}

		void update(const std_clock::timer &clk, const win32::input &inpt)
		{
			dt = clk.get_delta<std_clock::s>();
			tt = clk.get_total<std_clock::s>();

			using enum win32::input_button;

			if (inpt.is_button_down(escape))
			{
				close = true;
				std::println("⌚: {:>5.2f}s, ⏱️: {}ns, ⌨️: {:10.10}",
				             tt, dt, win32::to_string(escape));
			}

			constant.pos.x += static_cast<float>(dt);
			if (constant.pos.x > 1.5f)
			{
				constant.pos.x = -1.5f;
			}
		}

		[[nodiscard]] auto should_continue() const -> bool
		{
			return not close;
		}

		auto on_resize(std::uint32_t width, std::uint32_t height) -> bool
		{
			std::println("⌚: {:>5.2f}s, ⏱️: {}ns, Width: {:>5}, Height: {:>5}",
			             tt, dt, width, height);

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

		void setup_renderer(std::uint32_t &rndr)
		{
			setup_pipeline(rndr);
			setup_model(rndr);
			add_draw_cmds(rndr);
		}

		void setup_pipeline(std::uint32_t &rndr)
		{
			auto vert_shader_bin = read_file("shaders/basic_pc_shader.vert.spv");
			auto frag_shader_bin = read_file("shaders/basic_pc_shader.frag.spv");
		}

		void setup_model(std::uint32_t &rndr)
		{
		}

		void add_draw_cmds(std::uint32_t &rndr)
		{
		}

	private:
		bool close = false;
		double dt  = 0.f;
		double tt  = 0.f;

		uint32_t model_idx{};

		push_constant constant = {
			.pos = { -1.5f, 0.0f },
		};
	};
}