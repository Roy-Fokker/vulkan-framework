module;

export module vfw:commandpool;

import std;

import :types;

export namespace vfw
{
	class commandpool final
	{
	public:
		struct description
		{
			uint16_t max_frame_count;
			vk::Queue graphics_queue;
			uint32_t graphics_queue_index;
		};

	public:
		commandpool()                                  = delete;
		commandpool(const commandpool &src)            = delete;
		commandpool &operator=(const commandpool &src) = delete;
		commandpool(commandpool &&src)                 = delete;
		commandpool &operator=(commandpool &&src)      = delete;

		commandpool(vk::Device device, const description &desc)
			: device(device)
		{
			create_command_pool(desc);
			create_command_buffers(desc);
		}

		~commandpool()
		{
			device.waitIdle();

			destroy_command_buffers();
			destroy_command_pool();
		}

	private:
		void create_command_pool(const description &desc)
		{
			auto command_pool_info = vk::CommandPoolCreateInfo{
				.flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
				.queueFamilyIndex = desc.graphics_queue_index,
			};

			command_pool = device.createCommandPool(command_pool_info);
		}

		void destroy_command_buffers()
		{
			device.destroyCommandPool(command_pool);
		}

		void create_command_buffers(const description &desc)
		{
			auto cmd_buffer_alloc_info = vk::CommandBufferAllocateInfo{
				.commandPool        = command_pool,
				.level              = vk::CommandBufferLevel::ePrimary,
				.commandBufferCount = desc.max_frame_count
			};

			command_buffers = device.allocateCommandBuffers(cmd_buffer_alloc_info);
		}

		void destroy_command_pool()
		{
			// device.freeCommandBuffers(command_pool, command_buffers);
			command_buffers.clear();
		}

	private:
		vk::Device device;

		vk::CommandPool command_pool;
		std::vector<vk::CommandBuffer> command_buffers;
	};
}