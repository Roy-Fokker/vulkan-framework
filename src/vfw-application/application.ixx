module;

export module application;

import input;
import window;

import vfw;

export namespace app_base
{
	class application
	{
	public:
		application() = delete;
		explicit application(vfw::renderer &rndr)
		{
			on_keypress = [&](input::button button_, std::uint16_t scan_code, bool isKeyDown, std::uint16_t repeat_count) -> bool {
				std::println("⌚: {:>5.2f}s, ⏱️: {}ns, ⌨️: {:10.10}, 📑: {:>5}, 🔽: {:^5}, 🔁: {:>3}",
				             tt, dt, input::to_string(button_), scan_code, isKeyDown, repeat_count);

				switch (button_)
				{
					using enum input::button;

				case escape:
				{
					close = true;
					break;
				}
				}

				return true;
			};

			on_resize = [&](std::uint32_t width, std::uint32_t height) -> bool {
				std::println("⌚: {:>5.2f}s, ⏱️: {}ns, Width: {:>5}, Height: {:>5}",
				             tt, dt, width, height);

				return true;
			};

			on_activate = [&](win32::window::active_state is_active, bool minimized) -> bool {
				auto state = (is_active == win32::window::active_state::active) ? true : false;

				std::println("⌚: {:>5.2f}s, ⏱️: {}ns, Active: {:^5}, Minimized: {:^5}",
				             tt, dt, state, minimized);

				return true;
			};

			setup_renderer(rndr);
		}

		void update(double delta_time, double total_time)
		{
			dt = delta_time;
			tt = total_time;
		}

		[[nodiscard]] auto should_continue() const -> bool
		{
			return not close;
		}

		win32::window::keypress_callback on_keypress{};
		win32::window::resize_callback on_resize{};
		win32::window::activate_callback on_activate{};

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

		void setup_renderer(vfw::renderer &rndr)
		{
			rndr.set_clear_color({ 0.4f, 0.4f, 0.2f, 1.f });

			auto vert_shader_bin = read_file("shaders/basic_shader.vert.spv");
			auto frag_shader_bin = read_file("shaders/basic_shader.frag.spv");

			auto simple_pipeline = vfw::pipeline_description{
				.shaders = {
					{ vk::ShaderStageFlagBits::eVertex, vert_shader_bin },
					{ vk::ShaderStageFlagBits::eFragment, frag_shader_bin },
				},
				.input_attributes = vfw::vertex::get_attribute_descriptions(),
				.input_bindings   = vfw::vertex::get_binding_descriptions(),
				.topology         = { vk::PrimitiveTopology::eTriangleList },
				.polygon_mode     = { vk::PolygonMode::eFill },
				.cull_mode        = { vk::CullModeFlagBits::eBack },
				.front_face       = { vk::FrontFace::eClockwise },
			};

			rndr.add_pipeline(simple_pipeline);

			const std::vector<vfw::vertex> vertices = {
				{ { 0.0f, 0.5f }, { 1.0f, 0.0f, 0.0f } },
				{ { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
				{ { -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } },
			};

			auto vb_idx = rndr.add_buffer(vertices.size() * sizeof(vfw::vertex),
			                              reinterpret_cast<const void *>(vertices.data()),
			                              vk::BufferUsageFlagBits::eVertexBuffer,
			                              vk::SharingMode::eExclusive);

			rndr.draw_vb_cmd(vb_idx,
			                 static_cast<uint32_t>(vertices.size()),
			                 1, 0, 0);
		}

	private:
		bool close = false;
		double dt  = 0.f;
		double tt  = 0.f;
	};
}