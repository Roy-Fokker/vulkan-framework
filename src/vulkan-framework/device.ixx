module;

export module vfw:device;

import :instance;

export namespace vfw
{
	class device
	{
	public:
		explicit device(const instance *vfw_inst)
		{
			pick_physical_device(vfw_inst);
			create_logical_device(vfw_inst);

			auto props = vk_physical_device.getProperties2();
			std::println("Device Name: {}, Vulkan API: {}.{}",
			             props.properties.deviceName.data(),
			             vk::apiVersionMajor(props.properties.apiVersion),
			             vk::apiVersionMinor(props.properties.apiVersion));
		}
		~device()
		{
			vk_logical_device.destroy();
		}

		device() = delete;

		[[nodiscard]] auto get_queue_family() const -> queue_family
		{
			return qf;
		}

		auto get_device() const -> const vk::Device &
		{
			return vk_logical_device;
		}

		auto get_physical_device() const -> const vk::PhysicalDevice &
		{
			return vk_physical_device;
		}

		auto get_queues() const -> std::tuple<const vk::Queue &, const vk::Queue &>
		{
			return {
				vk_graphics_queue,
				vk_present_queue
			};
		}

	private:
		void pick_physical_device(const instance *vfw_inst);
		void create_logical_device(const instance *vfw_inst);

	private:
		vk::PhysicalDevice vk_physical_device;
		vk::Device vk_logical_device;
		vk::Queue vk_graphics_queue, vk_present_queue;
		queue_family qf;
	};
}

namespace
{
	const auto wanted_device_extensions = std::vector{
		VK_KHR_SWAPCHAIN_EXTENSION_NAME,
	};

	auto check_device_extension_support(const vk::PhysicalDevice &device, const std::vector<const char *> &extensions) -> bool
	{
		auto device_exts = device.enumerateDeviceExtensionProperties();

		auto trans_view = device_exts | std::views::transform([](const vk::ExtensionProperties &prop) -> const char * {
			return prop.extensionName.data();
		});

		auto supported_extensions = std::vector(std::begin(trans_view), std::end(trans_view));
		std::ranges::sort(supported_extensions, [](const char *a, const char *b) {
			return std::string_view(a) < std::string_view(b);
		});

		auto intersection = std::vector<const char *>{};
		std::ranges::set_intersection(extensions, supported_extensions, std::back_inserter(intersection), [](const char *a, const char *b) {
			return std::string_view(a) == std::string_view(b);
		});

		return (intersection.size() == extensions.size());
	}
}

using namespace vfw;

void device::pick_physical_device(const instance *vfw_inst)
{
	auto physical_devices     = vfw_inst->list_physical_devices();
	auto suitable_device_iter = std::ranges::find_if(physical_devices, [&](vk::PhysicalDevice &device) {
		auto srfc_dtls      = vfw_inst->query_surface_details(device);
		auto exts_supported = check_device_extension_support(device, wanted_device_extensions);
		auto que_fam        = vfw_inst->find_queue_family(device);

		return que_fam.is_complete() and exts_supported and not srfc_dtls.formats.empty() and not srfc_dtls.present_modes.empty();
	});
	if (suitable_device_iter == physical_devices.end())
	{
		throw std::runtime_error("Cannot find suitable physical device.");
	}
	vk_physical_device = *suitable_device_iter;
}

void device::create_logical_device(const instance *vfw_inst)
{
	qf = vfw_inst->find_queue_family(vk_physical_device);
	std::println("Queue Families: gfx:{}, present:{}",
	             qf.graphics_family.value_or(-1),
	             qf.present_family.value_or(-1));

	auto queue_array = qf.get_array();

	auto layers          = vfw_inst->get_layers();
	auto extensions      = wanted_device_extensions;
	auto device_features = vk::PhysicalDeviceFeatures{};

	auto device_createInfo = vk::DeviceCreateInfo{
		.queueCreateInfoCount    = static_cast<uint32_t>(queue_array.size()),
		.pQueueCreateInfos       = queue_array.data(),
		.enabledLayerCount       = static_cast<uint32_t>(layers.size()),
		.ppEnabledLayerNames     = layers.data(),
		.enabledExtensionCount   = static_cast<uint32_t>(extensions.size()),
		.ppEnabledExtensionNames = extensions.data(),
		.pEnabledFeatures        = &device_features
	};

	vk_logical_device = vk_physical_device.createDevice(device_createInfo);

	vk_graphics_queue = vk_logical_device.getQueue(qf.graphics_family.value(), 0);
	vk_present_queue  = vk_logical_device.getQueue(qf.present_family.value(), 0);
}
