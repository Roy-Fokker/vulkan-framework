module;

export module vfw:descriptors;

import std;

export namespace vfw
{
	struct descriptor_layout_builder final
	{
		std::vector<vk::DescriptorSetLayoutBinding> bindings{};

		void add_binding(uint32_t binding, vk::DescriptorType type)
		{
			bindings.push_back({
				.binding         = binding,
				.descriptorType  = type,
				.descriptorCount = 1,
			});
		}

		auto build(vk::Device device, vk::ShaderStageFlags shader_stages, vk::DescriptorSetLayoutCreateFlags flags = {}, void *pNext = nullptr)
			-> vk::DescriptorSetLayout
		{
			for (auto &b : bindings)
			{
				b.stageFlags |= shader_stages;
			}

			auto ci = vk::DescriptorSetLayoutCreateInfo{
				.pNext        = pNext,
				.flags        = flags,
				.bindingCount = static_cast<uint32_t>(bindings.size()),
				.pBindings    = bindings.data(),
			};

			return device.createDescriptorSetLayout(ci);
		}

		void clear()
		{
			bindings.clear();
		}
	};

	class descriptor_allocator final
	{
	public:
		struct pool_size_ratio
		{
			vk::DescriptorType type;
			float ratio;
		};

	public:
		descriptor_allocator()                                           = delete;
		descriptor_allocator(const descriptor_allocator &src)            = delete;
		descriptor_allocator &operator=(const descriptor_allocator &src) = delete;
		descriptor_allocator(descriptor_allocator &&src)                 = delete;
		descriptor_allocator &operator=(descriptor_allocator &&src)      = delete;

		descriptor_allocator(vk::Device device, uint32_t set_count, std::span<pool_size_ratio> pool_ratios)
			: device(device)
		{
			auto func = [&](auto &&pool) {
				return vk::DescriptorPoolSize{
					.type            = pool.type,
					.descriptorCount = static_cast<uint32_t>(pool.ratio * set_count),
				};
			};
			auto pool_sizes = pool_ratios | std::views::transform(func) | std::ranges::to<std::vector>();

			auto pi = vk::DescriptorPoolCreateInfo{
				.maxSets       = set_count,
				.poolSizeCount = static_cast<uint32_t>(pool_sizes.size()),
				.pPoolSizes    = pool_sizes.data(),
			};
			pool = device.createDescriptorPool(pi);
		}

		~descriptor_allocator()
		{
			device.destroyDescriptorPool(pool);
		}

		void clear_descriptors()
		{
			device.resetDescriptorPool(pool);
		}

		auto allocate(vk::DescriptorSetLayout layout) -> vk::DescriptorSet
		{
			auto ai = vk::DescriptorSetAllocateInfo{
				.descriptorPool     = pool,
				.descriptorSetCount = 1,
				.pSetLayouts        = &layout,
			};

			return device.allocateDescriptorSets(ai).front(); // TODO: this will need to change in future
		}

		void free(vk::DescriptorSet descriptor_set)
		{
			device.freeDescriptorSets(pool, descriptor_set);
		}

	private:
		vk::Device device;

		vk::DescriptorPool pool;
	};
}
