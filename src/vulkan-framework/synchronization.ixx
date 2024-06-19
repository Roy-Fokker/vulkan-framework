module;

export module vfw:synchronization;

import std;

export namespace vfw
{
	class frame_sync final
	{
	public:
		struct sync_flags
		{
			vk::Semaphore swapchain_semaphore;
			vk::Semaphore render_semaphore;
			vk::Fence render_fence;
		};

	public:
		frame_sync()                                 = delete;
		frame_sync(const frame_sync &src)            = delete;
		frame_sync &operator=(const frame_sync &src) = delete;
		frame_sync(frame_sync &&src)                 = delete;
		frame_sync &operator=(frame_sync &&src)      = delete;

		frame_sync(vk::Device device, uint32_t max_frame_count)
			: device(device)
		{
			auto make_sync_obj_fn = [&](auto &&sync_object) {
				auto semaphore_ci               = vk::SemaphoreCreateInfo{};
				sync_object.swapchain_semaphore = device.createSemaphore(semaphore_ci);
				sync_object.render_semaphore    = device.createSemaphore(semaphore_ci);

				auto fence_ci = vk::FenceCreateInfo{
					.flags = vk::FenceCreateFlagBits::eSignaled
				};

				sync_object.render_fence = device.createFence(fence_ci);
			};

			sync_objects.resize(max_frame_count);
			std::ranges::for_each(sync_objects, make_sync_obj_fn);
		}

		~frame_sync()
		{
			device.waitIdle();

			for (auto &&sync_object : sync_objects)
			{
				device.destroyFence(sync_object.render_fence);
				device.destroySemaphore(sync_object.render_semaphore);
				device.destroySemaphore(sync_object.swapchain_semaphore);
			}
		}

		[[nodiscard]] auto get_sync_objects_at(uint32_t frame_index) -> sync_flags
		{
			return sync_objects.at(frame_index);
		}

	private:
		vk::Device device;

		std::vector<sync_flags> sync_objects{};
	};
}