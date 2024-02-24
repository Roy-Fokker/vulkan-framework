module;

export module vfw:instance;

export namespace vfw
{
	struct surface_details
	{
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> present_modes;
	};

	struct queue_family
	{
		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;

		[[nodiscard]] auto is_complete() const -> bool
		{
			return graphics_family.has_value() and present_family.has_value();
		}

		[[nodiscard]] auto get_array() const -> std::vector<vk::DeviceQueueCreateInfo>
		{
			auto out            = std::vector<vk::DeviceQueueCreateInfo>{};
			auto queue_priority = 1.0f;

			if (graphics_family.has_value())
			{
				out.emplace_back(vk::DeviceQueueCreateInfo{
					.queueFamilyIndex = static_cast<uint32_t>(graphics_family.value()),
					.queueCount       = 1,
					.pQueuePriorities = &queue_priority });
			}

			if (is_complete() and present_family.value() != graphics_family.value())
			{
				out.emplace_back(vk::DeviceQueueCreateInfo{
					.queueFamilyIndex = static_cast<uint32_t>(present_family.value()),
					.queueCount       = 1,
					.pQueuePriorities = &queue_priority });
			}

			return out;
		}
	};

	class instance
	{
	public:
		explicit instance(HWND window_handle)
		{
			create_instance(window_handle);

#ifdef _DEBUG
			setup_debug_callback();
#endif

			create_surface(window_handle);
		}

		~instance()
		{
			vk_instance.destroySurfaceKHR(vk_surface);

#ifdef _DEBUG
			vk_instance.destroyDebugUtilsMessengerEXT(debug_messenger);
#endif

			vk_instance.destroy();
		}

		instance() = delete;

		auto list_physical_devices() const
		{
			return vk_instance.enumeratePhysicalDevices();
		}

		auto query_surface_details(const vk::PhysicalDevice &device) const -> surface_details
		{
			return surface_details{
				.capabilities  = device.getSurfaceCapabilitiesKHR(vk_surface),
				.formats       = device.getSurfaceFormatsKHR(vk_surface),
				.present_modes = device.getSurfacePresentModesKHR(vk_surface)
			};
		}

		auto find_queue_family(const vk::PhysicalDevice &device) const -> queue_family
		{
			auto out = queue_family{};

			auto queue_families = device.getQueueFamilyProperties();

			auto gf_fn = [&](auto &&pair) -> bool {
				auto &&[idx, qfp] = pair;

				return static_cast<bool>(qfp.queueFlags & vk::QueueFlagBits::eGraphics);
			};

			auto gf_rng = queue_families |
			              std::views::enumerate |
			              std::views::filter(gf_fn);

			if (not gf_rng.empty())
			{
				out.graphics_family = static_cast<uint32_t>(std::get<0>(*gf_rng.begin()));
			}

			auto pf_fn = [&](auto &&pair) -> bool {
				auto &&[idx, qfp] = pair;

				auto present_support = device.getSurfaceSupportKHR(static_cast<uint32_t>(idx), vk_surface);
				return static_cast<bool>(present_support);
			};

			auto pf_rng = queue_families |
			              std::views::enumerate |
			              std::views::filter(pf_fn);

			if (not pf_rng.empty())
			{
				out.present_family = static_cast<uint32_t>(std::get<0>(*pf_rng.begin()));
			}

			/* Old logic
			auto queue_family_iter = std::ranges::find_if(queue_families, [&](vk::QueueFamilyProperties &qf) -> bool {
				return static_cast<bool>(qf.queueFlags & vk::QueueFlagBits::eGraphics);
			});

			if (queue_family_iter != queue_families.end())
			{
				out.graphics_family = static_cast<uint32_t>(std::distance(queue_families.begin(), queue_family_iter));
			}

			auto queue_idx{ 0 };
			queue_family_iter = std::ranges::find_if(queue_families, [&]([[maybe_unused]] vk::QueueFamilyProperties &qf) -> bool {
				auto present_support = device.getSurfaceSupportKHR(queue_idx, vk_surface);
				queue_idx++;

				return static_cast<bool>(present_support);
			});

			if (queue_family_iter != queue_families.end())
			{
				out.present_family = static_cast<uint32_t>(std::distance(queue_families.begin(), queue_family_iter));
			}
			*/

			return out;
		}

		void update_surface(HWND window_handle)
		{
			vk_instance.destroySurfaceKHR(vk_surface);
			create_surface(window_handle);
		}

		auto get_surface() const -> const vk::SurfaceKHR &
		{
			return vk_surface;
		}

		auto get_layers() const -> std::vector<const char *>;

	private:
		void create_instance(HWND window_handle);
		void setup_debug_callback();
		void create_surface(HWND window_handle)
		{
			auto create_info = vk::Win32SurfaceCreateInfoKHR{
				.hinstance = GetModuleHandle(nullptr),
				.hwnd      = window_handle
			};

			vk_surface = vk_instance.createWin32SurfaceKHR(create_info);
		}

		vk::Instance vk_instance;
		vk::DebugUtilsMessengerEXT debug_messenger;
		vk::SurfaceKHR vk_surface;
	};
}

using namespace std::string_literals;
using namespace std::string_view_literals;

namespace
{
	// TODO: figure out constexpr/consteval way to do this.
	const auto wanted_instance_extensions = [] {
		auto exts = std::vector<std::string>{
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#ifdef _DEBUG
			VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
		};
		std::ranges::sort(exts);
		return exts;
	}();

	// TODO: figure out constexpr/consteval way to do this.
	const auto wanted_instance_layers = [] {
		auto lyrs = std::vector<std::string>{
#ifdef _DEBUG
			"VK_LAYER_KHRONOS_validation",
#endif
		};
		std::ranges::sort(lyrs);
		return lyrs;
	}();

	// TODO: would be better if output was vector<const char *>
	auto get_installed_extensions() -> std::vector<std::string>
	{
		auto exts = vk::enumerateInstanceExtensionProperties();
		auto out  = std::vector<std::string>{};

		std::ranges::transform(exts, std::back_inserter(out), [](vk::ExtensionProperties props) {
			return std::string(props.extensionName.data());
		});

		std::ranges::sort(out);

		return out;
	}

	// TODO: would be better if output was vector<const char *>
	auto get_installed_layers() -> std::vector<std::string>
	{
		auto lyrs = vk::enumerateInstanceLayerProperties();
		auto out  = std::vector<std::string>{};

		std::ranges::transform(lyrs, std::back_inserter(out), [](vk::LayerProperties props) {
			return std::string(props.layerName.data());
		});

		std::ranges::sort(out);

		return out;
	}

	// TODO: there has to be a better way to do this.
	auto convert_to_vec_char(std::vector<std::string> &in) -> std::vector<const char *>
	{
		auto out = std::vector<const char *>{};
		std::ranges::transform(in, std::back_inserter(out), [](std::string &s) {
			auto c_str = new char[s.size()];
			std::strcpy(c_str, s.c_str());
			return c_str;
		});
		return out;
	}

	auto get_window_name(HWND handle) -> std::string
	{
		auto len  = static_cast<size_t>(GetWindowTextLengthA(handle)) + 1;
		auto name = ""s;

		name.resize(len);
		GetWindowTextA(handle, &name[0], static_cast<int>(len)); // has to be explicit call to xxA function because Vulkan expect char array.
		name.resize(len - 1);

		return name;
	}

}

#ifdef _DEBUG
static auto debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                           [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
                           const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                           [[maybe_unused]] void *pUserData)
	-> vk::Bool32
{
	if (messageSeverity > VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
	{
		std::println("Debug Message: {}\n", pCallbackData->pMessage);
	}
	return VK_FALSE;
}

extern auto vkCreateDebugUtilsMessengerEXT(VkInstance instance,
                                           const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
                                           const VkAllocationCallbacks *pAllocator,
                                           VkDebugUtilsMessengerEXT *pDebugMessenger)
	-> VkResult
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

extern void vkDestroyDebugUtilsMessengerEXT(VkInstance instance,
                                            VkDebugUtilsMessengerEXT debugMessenger,
                                            const VkAllocationCallbacks *pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		func(instance, debugMessenger, pAllocator);
	}
}
#endif

using namespace vfw;

auto instance::get_layers() const -> std::vector<const char *>
{
	auto installed_layers = get_installed_layers();
	auto available_layers = std::vector<std::string>{};
	std::ranges::set_intersection(installed_layers, wanted_instance_layers, std::back_inserter(available_layers));

	auto lyrs = convert_to_vec_char(available_layers);

	return lyrs;
}

void instance::create_instance(const HWND window_handle)
{
	const auto name    = get_window_name(window_handle);
	const auto version = VK_MAKE_VERSION(0, 0, 1);
	const auto engine  = "VulkanFramework"sv;

	std::println("Window Name: {}, Version, {}, Engine Name: {}", name, version, engine);

	auto get_exts = [&]() {
		auto installed_extensions = get_installed_extensions();
		auto available_extensions = std::vector<std::string>{};
		std::ranges::set_intersection(installed_extensions, wanted_instance_extensions, std::back_inserter(available_extensions));

		auto exts = convert_to_vec_char(available_extensions);

		return exts;
	};

	auto app_info = vk::ApplicationInfo{
		.pApplicationName   = name.data(),
		.applicationVersion = version,
		.pEngineName        = engine.data(),
		.engineVersion      = version,
		.apiVersion         = VK_API_VERSION_1_3
	};

	auto lyrs = get_layers();
	auto exts = get_exts();

	auto create_info = vk::InstanceCreateInfo{
		.pApplicationInfo        = &app_info,
		.enabledLayerCount       = static_cast<uint32_t>(lyrs.size()),
		.ppEnabledLayerNames     = lyrs.data(),
		.enabledExtensionCount   = static_cast<uint32_t>(exts.size()),
		.ppEnabledExtensionNames = exts.data()
	};

	try
	{
		vk_instance = vk::createInstance(create_info);
	}
	catch (vk::SystemError &err)
	{
		std::cerr << std::format("Vulkan System Error: {}\n", err.what());
		throw err;
	}
}

void instance::setup_debug_callback()
{
	using sev  = vk::DebugUtilsMessageSeverityFlagBitsEXT;
	using type = vk::DebugUtilsMessageTypeFlagBitsEXT;

	auto createInfo = vk::DebugUtilsMessengerCreateInfoEXT{
		.messageSeverity = { sev::eWarning | sev::eError },
		.messageType     = { type::eGeneral | type::ePerformance | type::eValidation },
		.pfnUserCallback = &debug_callback
	};

	debug_messenger = vk_instance.createDebugUtilsMessengerEXT(createInfo);
}