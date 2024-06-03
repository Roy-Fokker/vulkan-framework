module;

#include <Windows.h>
#include <VkBootstrap.h>

#include <vma/vk_mem_alloc.h>

export module vfw:context;

import std;

import :types;

export namespace vfw
{
	constexpr auto use_vulkan_validation_layers{
#ifdef _DEBUG
		true
#else
		false
#endif
	};

	class context final
	{
	public:
		context(HWND hWnd)
		{
			auto vkb_inst = create_instance();
			create_surface(hWnd);
			pick_device_and_queue(vkb_inst);
			create_gpu_memory_allocator();

			auto [width, height] = get_window_size(hWnd);
			create_swapchain(width, height);
		}

		~context()
		{
			destroy_swapchain();

			destroy_gpu_memory_allocator();
			destroy_surface();
			device.destroy();
			vkb::destroy_debug_utils_messenger(instance, debug_messenger);
			instance.destroy();
		}

		void window_resized(std::uint32_t width, std::uint32_t height)
		{
			destroy_swapchain();

			create_swapchain(width, height);
		}

	private:
		auto create_instance() -> vkb::Instance
		{
			auto builder = vkb::InstanceBuilder{};

			auto vkb_ib_ret = builder.set_app_name("VulkanFrameworkEngine")
			                      .request_validation_layers(use_vulkan_validation_layers)
			                      .use_default_debug_messenger()
			                      .require_api_version(1, 3, 0)
			                      .build();
			auto vkb_inst = vkb_ib_ret.value();

			instance        = vkb_inst.instance;
			debug_messenger = vkb_inst.debug_messenger;

			return vkb_inst;
		}

		void create_surface(HWND hWnd)
		{
			auto create_info = vk::Win32SurfaceCreateInfoKHR{
				.hinstance = GetModuleHandle(nullptr),
				.hwnd      = hWnd
			};

			surface = instance.createWin32SurfaceKHR(create_info);
		}

		void destroy_surface()
		{
			instance.destroySurfaceKHR(surface);
		}

		void pick_device_and_queue(vkb::Instance vkb_inst)
		{
			auto features1_3 = vk::PhysicalDeviceVulkan13Features{
				.synchronization2 = true,
				.dynamicRendering = true,
			};

			auto features1_2 = vk::PhysicalDeviceVulkan12Features{
				.descriptorIndexing                       = true,
				.descriptorBindingPartiallyBound          = true,
				.descriptorBindingVariableDescriptorCount = true,
				.runtimeDescriptorArray                   = true,
				.bufferDeviceAddress                      = true,
			};

			auto phy_dev_selector = vkb::PhysicalDeviceSelector{ vkb_inst };
			auto phy_dev_ret      = phy_dev_selector
			                       .set_minimum_version(1, 3)
			                       .set_required_features_13(features1_3)
			                       .set_required_features_12(features1_2)
			                       .set_surface(surface)
			                       .select()
			                       .value();
			auto device_builder = vkb::DeviceBuilder{ phy_dev_ret };
			auto vkb_device     = device_builder.build().value();

			chosen_gpu            = phy_dev_ret.physical_device;
			device                = vkb_device.device;
			graphics_queue        = vkb_device.get_queue(vkb::QueueType::graphics).value();
			graphics_queue_family = vkb_device.get_queue_index(vkb::QueueType::graphics).value();
		}

		void create_gpu_memory_allocator()
		{
			auto allocator_info = VmaAllocatorCreateInfo{
				.flags          = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
				.physicalDevice = chosen_gpu,
				.device         = device,
				.instance       = instance,
			};

			vmaCreateAllocator(&allocator_info, &vma_allocator);
		}

		void destroy_gpu_memory_allocator()
		{
			vmaDestroyAllocator(vma_allocator);
		}

		auto get_window_size(HWND hWnd) -> const std::array<uint16_t, 2>
		{
			RECT rect{};
			GetClientRect(hWnd, &rect);

			return {
				static_cast<std::uint16_t>(rect.right - rect.left),
				static_cast<std::uint16_t>(rect.bottom - rect.top)
			};
		}

		void create_swapchain(std::uint32_t width, std::uint32_t height)
		{
			auto sc_builder = vkb::SwapchainBuilder{ chosen_gpu, device, surface };
			auto vkb_sc     = sc_builder
			                  .set_desired_format({
								  .format     = VK_FORMAT_B8G8R8A8_UNORM,
								  .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR,
							  })
			                  .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
			                  .set_desired_extent(width, height)
			                  .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
			                  .build()
			                  .value();

			swapchain        = vkb_sc.swapchain;
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

			device.destroySwapchainKHR(swapchain);
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

			vmaCreateImage(vma_allocator,
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
			vmaDestroyImage(vma_allocator, draw_image.image, draw_image.allocation);
		}

	private:
		vk::Instance instance;
		vk::DebugUtilsMessengerEXT debug_messenger;
		vk::SurfaceKHR surface;
		vk::PhysicalDevice chosen_gpu;
		vk::Device device;
		vk::Queue graphics_queue;
		std::uint32_t graphics_queue_family;

		VmaAllocator vma_allocator;

		vk::SwapchainKHR swapchain;
		vk::Extent2D swapchain_extent;
		std::vector<vk::Image> swapchain_images;
		std::vector<vk::ImageView> swapchain_imageviews;

		types::allocated_image draw_image;
	};
}
