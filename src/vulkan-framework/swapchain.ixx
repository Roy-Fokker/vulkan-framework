module;

#include <cassert>

#include <VkBootstrap.h>

#include <vma/vk_mem_alloc.h>

export module vfw:swapchain;

import std;

import :types;

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

		swapchain(vk::Device device, VmaAllocator allocator, const description &desc)
			: device(device), allocator(allocator)
		{
			create_swapchain(desc);
		}

		~swapchain()
		{
			device.waitIdle();
			destroy_swapchain();
		}

		[[nodiscard]] auto get_image_count() -> uint16_t
		{
			assert(swapchain_imageviews.size() < UINT16_MAX);
			return static_cast<uint16_t>(swapchain_imageviews.size());
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

			allocate_image_memory();
		}

		void destroy_swapchain()
		{
			free_image_memory();

			for (auto &&iv : swapchain_imageviews)
			{
				device.destroyImageView(iv);
			}
			swapchain_imageviews.clear();

			device.destroySwapchainKHR(swapchainKHR);
		}

		void allocate_image_memory()
		{
			auto draw_image_extent = vk::Extent3D{
				swapchain_extent.width,
				swapchain_extent.height,
				1,
			};

			draw_image = types::allocated_image{
				.extent = draw_image_extent,
				.format = vk::Format::eR16G16B16A16Sfloat,
			};

			auto draw_image_usage = vk::ImageUsageFlags{
				vk::ImageUsageFlagBits::eTransferSrc |
				vk::ImageUsageFlagBits::eTransferDst |
				vk::ImageUsageFlagBits::eStorage |
				vk::ImageUsageFlagBits::eColorAttachment
			};

			auto image_ci = vk::ImageCreateInfo{
				.imageType   = vk::ImageType::e2D,
				.format      = draw_image.format,
				.extent      = draw_image.extent,
				.mipLevels   = 1,
				.arrayLayers = 1,
				.samples     = vk::SampleCountFlagBits::e1,
				.tiling      = vk::ImageTiling::eOptimal,
				.usage       = draw_image_usage,
			};

			auto image_ai = VmaAllocationCreateInfo{
				.usage         = VMA_MEMORY_USAGE_GPU_ONLY,
				.requiredFlags = VkMemoryPropertyFlags{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT },
			};

			vmaCreateImage(allocator,
			               reinterpret_cast<VkImageCreateInfo *>(&image_ci),
			               &image_ai,
			               reinterpret_cast<VkImage *>(&draw_image.image),
			               &draw_image.allocation,
			               nullptr);

			auto view_ci = vk::ImageViewCreateInfo{
				.image            = draw_image.image,
				.viewType         = vk::ImageViewType::e2D,
				.format           = draw_image.format,
				.subresourceRange = {
					.aspectMask     = vk::ImageAspectFlagBits::eColor,
					.baseMipLevel   = 0,
					.levelCount     = 1,
					.baseArrayLayer = 0,
					.layerCount     = 1,
				},
			};

			draw_image.view = device.createImageView(view_ci);
		}

		void free_image_memory()
		{
			device.destroyImageView(draw_image.view);
			vmaDestroyImage(allocator, draw_image.image, draw_image.allocation);
		}

	private:
		vk::Device device;
		VmaAllocator allocator;

		vk::SwapchainKHR swapchainKHR;
		vk::Extent2D swapchain_extent;
		std::vector<vk::Image> swapchain_images;
		std::vector<vk::ImageView> swapchain_imageviews;

		types::allocated_image draw_image;
	};
}