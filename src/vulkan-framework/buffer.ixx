module;

export module vfw:buffer;

import :device;

export namespace vfw
{
	class buffer
	{
	public:
		struct description
		{
			size_t buffer_size;
			const void *buffer_data;
			vk::BufferUsageFlags usage;
			vk::SharingMode sharing_mode;
		};

	public:
		buffer() = delete;
		explicit buffer(const vfw::device *device, const description &desc)
		{
			vk_device = device->get_device();

			create_buffer(desc);

			auto mem_reqs = vk_device.getBufferMemoryRequirements(vk_buffer);
			auto mem_type = find_memory_type(device->get_physical_device(),
			                                 mem_reqs.memoryTypeBits,
			                                 vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
			allocate_memory(mem_reqs, mem_type);
			update_buffer(desc.buffer_size, desc.buffer_data);
		}

		~buffer()
		{
			vk_device.freeMemory(vk_buffer_memory);
			vk_device.destroyBuffer(vk_buffer);
		}

		// TODO: check to make sure buffer_size doesn't exceed what was allocated for this object.
		void update_buffer(size_t buffer_size, const void *buffer_data)
		{
			auto data = vk_device.mapMemory(vk_buffer_memory, 0, buffer_size);
			std::memcpy(data, buffer_data, buffer_size);
			vk_device.unmapMemory(vk_buffer_memory);
		}

		[[nodiscard]] auto get_buffer() const -> vk::Buffer
		{
			return vk_buffer;
		}

	private:
		void create_buffer(const description &desc)
		{
			auto buffer_info = vk::BufferCreateInfo{
				.size        = desc.buffer_size,
				.usage       = desc.usage,
				.sharingMode = desc.sharing_mode,
			};

			vk_buffer = vk_device.createBuffer(buffer_info);
		}

		auto find_memory_type(const vk::PhysicalDevice &device, uint32_t type_filter, vk::MemoryPropertyFlags properties) -> uint32_t
		{
			auto phys_mem_props = device.getMemoryProperties();

			std::println("Wanted Mem Prop: {}", vk::to_string(properties));
			auto filter_fn = [&](auto &&pair) {
				auto &&[idx, mem_type] = pair;
				std::println("Index: {}, Memory Props: {}", idx, vk::to_string(mem_type.propertyFlags));
				return (type_filter & (1 << idx)) and
				       ((mem_type.propertyFlags & properties) == properties);
			};

			// TODO: there must be a way to use ranges to get the 1st filtered value from enumeration view
			auto ev = phys_mem_props.memoryTypes |
			          std::views::enumerate |
			          std::views::filter(filter_fn);

			if (ev.empty())
			{
				std::println("Did not find compatible memory type.");
				throw std::runtime_error("Did not find compatible memory type.");
			}

			auto mem_type = std::get<0>(*ev.begin());
			std::println("Compatible memory type: {}", mem_type);
			return static_cast<uint32_t>(mem_type);
		}

		void allocate_memory(vk::MemoryRequirements memory_requirements, uint32_t memory_type)
		{
			auto allocate_info = vk::MemoryAllocateInfo{
				.allocationSize  = memory_requirements.size,
				.memoryTypeIndex = memory_type,
			};

			vk_buffer_memory = vk_device.allocateMemory(allocate_info);
			vk_device.bindBufferMemory(vk_buffer, vk_buffer_memory, 0);
		}

	private:
		vk::Device vk_device;
		vk::DeviceMemory vk_buffer_memory;
		vk::Buffer vk_buffer;
	};
}