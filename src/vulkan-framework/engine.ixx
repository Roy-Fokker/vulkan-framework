module;

#include <Windows.h>
#include <VkBootstrap.h>

// Vulkan Memory Allocator
#include <vma/vk_mem_alloc.h>

export module vfw:engine;

import std;

export namespace vfw
{
	constexpr auto use_vulkan_validation_layers{
#ifdef _DEBUG
		true
#else
		false
#endif
	};

	class engine final
	{
	public:
		engine(HWND hWnd)
		{
			auto vkb_inst = create_instance();
			create_surface(hWnd);
			pick_device_and_queue(vkb_inst);
			create_gpu_memory_allocator();
		}

		~engine() = default;

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

	private:
		vk::Instance instance;
		vk::DebugUtilsMessengerEXT debug_messenger;
		vk::SurfaceKHR surface;
		vk::PhysicalDevice chosen_gpu;
		vk::Device device;
		vk::Queue graphics_queue;
		std::uint32_t graphics_queue_family;

		VmaAllocator vma_allocator;
	};
}