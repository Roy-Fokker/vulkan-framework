module;

export module vfw:renderer;

import :instance;

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
	};
}

namespace
{

}

using namespace vfw;

renderer::renderer(HWND window_handle)
{

	vk_instance = std::make_unique<instance>(window_handle);
}

void renderer::draw_frame()
{
}