module;

export module vfw:renderer;

import :instance;
import :device;

export namespace vfw
{
	class renderer
	{
	public:
		renderer(HWND window_handle);
		~renderer() = default;

		void draw_frame();

	private:
		std::unique_ptr<instance> vk_instance = nullptr;
		std::unique_ptr<device> vk_device     = nullptr;
	};
}

namespace
{

}

using namespace vfw;

renderer::renderer(HWND window_handle)
{
	vk_instance = std::make_unique<instance>(window_handle);
	vk_device   = std::make_unique<device>(vk_instance.get());
}

void renderer::draw_frame()
{
}