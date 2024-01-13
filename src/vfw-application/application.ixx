module;

export module application;

import input;
import window;

export namespace app_base
{
	class application
	{
	public:
		application()
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
		bool close = false;
		double dt  = 0.f;
		double tt  = 0.f;
	};
}