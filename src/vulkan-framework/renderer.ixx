module;

export module vfw:renderer;

import :instance;
import :device;
import :swap_chain;

export namespace vfw
{
	class renderer
	{
	public:
		renderer(HWND window_handle)
		{
			vk_instance   = std::make_unique<instance>(window_handle);
			vk_device     = std::make_unique<device>(vk_instance.get());
			vk_swap_chain = std::make_unique<swap_chain>(vk_instance.get(), vk_device.get());
		}
		~renderer() = default;

		void draw_frame()
		{
		}

	private:
		std::unique_ptr<instance> vk_instance     = nullptr;
		std::unique_ptr<device> vk_device         = nullptr;
		std::unique_ptr<swap_chain> vk_swap_chain = nullptr;
	};
}
