module;

export module vfw:renderer;

import :instance;
import :device;
import :swap_chain;

export namespace vfw
{
	class renderer
	{
	public:
		renderer(HWND window_handle)
		{
			vk_instance   = std::make_unique<instance>(window_handle);
			vk_device     = std::make_unique<device>(vk_instance.get());
			vk_swap_chain = std::make_unique<swap_chain>(vk_instance.get(), vk_device.get());

			max_frames_in_flight = vk_swap_chain->get_image_count();

			create_command_pool();
			create_command_buffers();
			create_synchronization_objects();
		}
		~renderer()
		{
			auto ldevice = vk_device->get_device();
			ldevice.waitIdle();

			destroy_synchronization_objects();

			ldevice.destroyCommandPool(command_pool);
		}

		void set_clear_color(const std::array<float, 4> &color)
		{
			clear_color = color;
		}

		void draw_frame()
		{
			auto result                    = vk::Result{};
			auto image_index               = 0u;
			auto in_flight_fence           = in_flight_fences.at(current_frame);
			auto image_available_semaphore = image_available_semaphores.at(current_frame);
			auto render_finished_semaphore = render_finished_semaphores.at(current_frame);
			auto command_buffer            = command_buffers.at(current_frame);
			auto sc                        = vk_swap_chain->get();
			auto dev                       = vk_device->get_device();

			result = dev.waitForFences(in_flight_fence, true, UINT64_MAX);

			std::tie(result, image_index) = dev.acquireNextImageKHR(sc, UINT64_MAX, image_available_semaphore, VK_NULL_HANDLE);
			if (result == vk::Result::eErrorOutOfDateKHR or result == vk::Result::eSuboptimalKHR)
			{
				std::println("Did window resize?");
				return;
			}

			dev.resetFences(in_flight_fence);

			record_command_buffer(command_buffer, image_index);

			auto wait_stage = vk::PipelineStageFlags{
				vk::PipelineStageFlagBits::eColorAttachmentOutput
			};

			auto submit_info = vk::SubmitInfo{
				.waitSemaphoreCount   = 1,
				.pWaitSemaphores      = &image_available_semaphore,
				.pWaitDstStageMask    = &wait_stage,
				.commandBufferCount   = 1,
				.pCommandBuffers      = &command_buffer,
				.signalSemaphoreCount = 1,
				.pSignalSemaphores    = &render_finished_semaphore,
			};

			auto present_info = vk::PresentInfoKHR{
				.waitSemaphoreCount = 1,
				.pWaitSemaphores    = &render_finished_semaphore,
				.swapchainCount     = 1,
				.pSwapchains        = &sc,
				.pImageIndices      = &image_index,
			};
			auto &&[gq, pq] = vk_device->get_queues();

			gq.submit({ submit_info }, in_flight_fence);
			result = pq.presentKHR(present_info);

			current_frame = (current_frame + 1) % max_frames_in_flight; // 0 ... max_frame
		}

	private:
		void create_command_pool()
		{
			auto ldevice              = vk_device->get_device();
			auto queue_family_indices = vk_device->get_queue_family();

			auto command_pool_info = vk::CommandPoolCreateInfo{
				.flags            = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
				.queueFamilyIndex = queue_family_indices.graphics_family.value(),
			};

			command_pool = ldevice.createCommandPool(command_pool_info);
		}

		void create_command_buffers()
		{
			auto ldevice = vk_device->get_device();

			auto cmd_buffer_alloc_info = vk::CommandBufferAllocateInfo{
				.commandPool        = command_pool,
				.level              = vk::CommandBufferLevel::ePrimary,
				.commandBufferCount = max_frames_in_flight
			};

			command_buffers = ldevice.allocateCommandBuffers(cmd_buffer_alloc_info);
		}

		void create_synchronization_objects()
		{
			auto ldevice = vk_device->get_device();
			image_available_semaphores.resize(max_frames_in_flight);
			render_finished_semaphores.resize(max_frames_in_flight);
			in_flight_fences.resize(max_frames_in_flight);

			for (auto &&[image_available_semaphore, render_finished_semaphore, in_flight_fence] : std::views::zip(image_available_semaphores, render_finished_semaphores, in_flight_fences))
			{
				auto semaphore_ci         = vk::SemaphoreCreateInfo{};
				image_available_semaphore = ldevice.createSemaphore(semaphore_ci);
				render_finished_semaphore = ldevice.createSemaphore(semaphore_ci);

				auto fence_ci = vk::FenceCreateInfo{
					.flags = vk::FenceCreateFlagBits::eSignaled
				};
				in_flight_fence = ldevice.createFence(fence_ci);
			}
		}

		void destroy_synchronization_objects()
		{
			auto ldevice = vk_device->get_device();

			for (auto &&[image_available_semaphore, render_finished_semaphore, in_flight_fence] : std::views::zip(image_available_semaphores, render_finished_semaphores, in_flight_fences))
			{
				ldevice.destroyFence(in_flight_fence);
				ldevice.destroySemaphore(render_finished_semaphore);
				ldevice.destroySemaphore(image_available_semaphore);
			}
		}

		void record_command_buffer(vk::CommandBuffer &cb, uint32_t image_index)
		{
			cb.reset();

			auto result        = vk::Result{};
			auto cb_begin_info = vk::CommandBufferBeginInfo{};
			result             = cb.begin(&cb_begin_info);
			if (result != vk::Result::eSuccess)
			{
				std::println("Failed to begin recording command buffer, index:{}", image_index);
				return;
			}

			record_renderpass(cb, image_index);

			cb.end();
		}

		void record_renderpass(vk::CommandBuffer &cb, uint32_t image_index)
		{
			auto extent      = vk_swap_chain->get_extent();
			auto clear_value = vk::ClearValue{
				.color = clear_color
			};

			auto rp_begin_info = vk::RenderPassBeginInfo{
				.renderPass  = vk_swap_chain->get_render_pass(),
				.framebuffer = vk_swap_chain->frame_buffer(image_index),
				.renderArea  = {
					 .offset = { 0, 0 },
					 .extent = extent },
				.clearValueCount = 1,
				.pClearValues    = &clear_value,
			};

			cb.beginRenderPass(rp_begin_info, vk::SubpassContents::eInline);
			{
			}
			cb.endRenderPass();
		}

	private:
		std::unique_ptr<instance> vk_instance     = nullptr;
		std::unique_ptr<device> vk_device         = nullptr;
		std::unique_ptr<swap_chain> vk_swap_chain = nullptr;

		uint32_t max_frames_in_flight = 0;
		uint32_t current_frame        = 0;

		vk::CommandPool command_pool;
		std::vector<vk::CommandBuffer> command_buffers;

		std::vector<vk::Semaphore> image_available_semaphores;
		std::vector<vk::Semaphore> render_finished_semaphores;
		std::vector<vk::Fence> in_flight_fences;

		std::array<float, 4> clear_color = { 0.2f, 0.2f, 0.4f, 1.f };
	};
}
