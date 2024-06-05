module;

#include <Windows.h>
#include <VkBootstrap.h>

#include <vma/vk_mem_alloc.h>

export module vfw:context;

import std;

import :synchronization;
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
		context()                              = delete;
		context(const context &src)            = delete;
		context &operator=(const context &src) = delete;
		context(context &&src)                 = delete;
		context &operator=(context &&src)      = delete;

		context(HWND hWnd)
		{
			auto vkb_inst = create_instance();
			create_surface(hWnd);
			pick_device_and_queue(vkb_inst);
			create_gpu_memory_allocator();
		}

		~context()
		{
			device.waitIdle();

			destroy_gpu_memory_allocator();
			destroy_surface();
			device.destroy();
			vkb::destroy_debug_utils_messenger(instance, debug_messenger);
			instance.destroy();
		}

		void window_resized([[maybe_unused]] std::uint16_t width, [[maybe_unused]] std::uint16_t height)
		{
			device.waitIdle();
		}

		[[nodiscard]] auto get_device() -> vk::Device
		{
			return device;
		}

		[[nodiscard]] auto get_mem_allocator() -> VmaAllocator
		{
			return vma_allocator;
		}

		[[nodiscard]] auto get_surface() -> vk::SurfaceKHR
		{
			return surface;
		}

		[[nodiscard]] auto get_chosen_gpu() -> vk::PhysicalDevice
		{
			return chosen_gpu;
		}

		[[nodiscard]] auto get_graphics_queue() -> vk::Queue
		{
			return graphics_queue;
		}

		[[nodiscard]] auto get_graphics_queue_family() -> std::uint32_t
		{
			return graphics_queue_family;
		}

		void submit(vk::CommandBuffer &cb, const frame_sync::sync_flags &fs_sf)
		{
			auto wait_info = vk::SemaphoreSubmitInfo{
				.semaphore   = fs_sf.swapchain_semaphore,
				.value       = 1,
				.stageMask   = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
				.deviceIndex = 0,
			};

			auto signal_info = vk::SemaphoreSubmitInfo{
				.semaphore   = fs_sf.render_semaphore,
				.value       = 1,
				.stageMask   = vk::PipelineStageFlagBits2::eAllGraphics,
				.deviceIndex = 0,
			};

			auto cb_submit_info = vk::CommandBufferSubmitInfo{
				.commandBuffer = cb,
				.deviceMask    = 0,
			};

			auto submit_info = vk::SubmitInfo2{
				.waitSemaphoreInfoCount   = 1,
				.pWaitSemaphoreInfos      = &wait_info,
				.commandBufferInfoCount   = 1,
				.pCommandBufferInfos      = &cb_submit_info,
				.signalSemaphoreInfoCount = 1,
				.pSignalSemaphoreInfos    = &signal_info,
			};

			graphics_queue.submit2(submit_info, fs_sf.render_fence);
		}

		void present(const vk::SwapchainKHR &swapchainKHR, uint32_t image_index, const vk::Semaphore &render_finished_semaphore)
		{
			auto present_info = vk::PresentInfoKHR{
				.waitSemaphoreCount = 1,
				.pWaitSemaphores    = &render_finished_semaphore,
				.swapchainCount     = 1,
				.pSwapchains        = &swapchainKHR,
				.pImageIndices      = &image_index,
			};

			auto result = graphics_queue.presentKHR(present_info);
			assert(result == vk::Result::eSuccess);
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

	private:
		vk::Instance instance{ nullptr };
		vk::DebugUtilsMessengerEXT debug_messenger;
		vk::SurfaceKHR surface;
		vk::PhysicalDevice chosen_gpu;
		vk::Device device;
		vk::Queue graphics_queue;
		std::uint32_t graphics_queue_family;

		VmaAllocator vma_allocator;
	};
}
