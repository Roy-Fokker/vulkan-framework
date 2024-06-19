module;

#include <cassert>

#include <VkBootstrap.h>

export module vfw:swapchain;

import std;

export namespace vfw
{
	class swapchain final
	{
	public:
		struct description
		{
			uint16_t width;
			uint16_t height;
			vk::SurfaceKHR surface;
			vk::PhysicalDevice chosen_gpu;
		};

	public:
		swapchain()                                = delete;
		swapchain(const swapchain &src)            = delete;
		swapchain &operator=(const swapchain &src) = delete;
		swapchain(swapchain &&src)                 = delete;
		swapchain &operator=(swapchain &&src)      = delete;

		swapchain(vk::Device device, const description &desc)
			: device(device)
		{
			create_swapchain(desc);
		}

		~swapchain()
		{
			device.waitIdle();
			destroy_swapchain();
		}

		[[nodiscard]] auto get_image_count() -> uint32_t
		{
			assert(swapchain_imageviews.size() < UINT16_MAX);
			return static_cast<uint32_t>(swapchain_imageviews.size());
		}

		[[nodiscard]] auto next_image_index(vk::Semaphore image_semaphore) -> uint32_t
		{
			const auto wait_time       = 1'000'000'000u;
			auto [result, image_index] = device.acquireNextImageKHR(swapchainKHR,
			                                                        wait_time,
			                                                        image_semaphore,
			                                                        VK_NULL_HANDLE);

			return image_index;
		}

		[[nodiscard]] auto get_image(uint32_t image_index) -> vk::Image &
		{
			return swapchain_images.at(image_index);
		}

		[[nodiscard]] auto get_swapchain() -> vk::SwapchainKHR
		{
			return swapchainKHR;
		}

		[[nodiscard]] auto get_size() -> vk::Extent2D
		{
			return swapchain_extent;
		}

	private:
		void create_swapchain(const description &desc)
		{
			auto sc_builder = vkb::SwapchainBuilder{ desc.chosen_gpu, device, desc.surface };
			auto vkb_sc     = sc_builder
			                  .set_desired_format({
								  .format     = VK_FORMAT_B8G8R8A8_UNORM,
								  .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
							  })
			                  .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
			                  .set_desired_extent(desc.width, desc.height)
			                  .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			                  .build()
			                  .value();

			swapchainKHR     = vkb_sc.swapchain;
			swapchain_extent = vkb_sc.extent;

			// TODO: figure out why trying to use std::ranges causes the compiler to ICE.
			for (auto vk_i : vkb_sc.get_images().value())
			{
				swapchain_images.push_back(vk_i);
			}

			for (auto vk_iv : vkb_sc.get_image_views().value())
			{
				swapchain_imageviews.push_back(vk_iv);
			}
		}

		void destroy_swapchain()
		{
			std::ranges::for_each(swapchain_imageviews, [&](auto &&iv) {
				device.destroyImageView(iv);
			});
			swapchain_imageviews.clear();

			device.destroySwapchainKHR(swapchainKHR);
		}

	private:
		vk::Device device;

		vk::SwapchainKHR swapchainKHR;
		vk::Extent2D swapchain_extent;
		std::vector<vk::Image> swapchain_images;
		std::vector<vk::ImageView> swapchain_imageviews;
	};
}