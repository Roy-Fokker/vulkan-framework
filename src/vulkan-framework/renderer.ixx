module;

export module vfw:renderer;

import :instance;
import :device;
import :swap_chain;
import :pipeline;
import :buffer;

export namespace vfw
{
	using pipeline_description = pipeline::description;

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

		void add_pipeline(const pipeline_description &desc)
		{
			auto ldevice = vk_device->get_device();
			auto rp      = vk_swap_chain->get_render_pass();
			gfx_pipeline = std::make_unique<pipeline>(ldevice, rp, desc);
		}

		[[nodiscard]] auto add_buffer(size_t buffer_size,
		                              const void *buffer_data,
		                              vk::BufferUsageFlagBits usage,
		                              vk::SharingMode sharing_mode)
			-> uint32_t
		{
			gfx_buffers.emplace_back(vk_device.get(),
			                         buffer::description{
										 .buffer_size  = buffer_size,
										 .buffer_data  = buffer_data,
										 .usage        = usage,
										 .sharing_mode = sharing_mode,
									 });
			return static_cast<uint32_t>(gfx_buffers.size() - 1);
		}

		void draw_vb_cmd(uint32_t buffer_idx,
		                 uint32_t vertex_count,
		                 uint32_t instance_count,
		                 uint32_t vertex_offset,
		                 uint32_t index_offset)
		{
			draw_vb_commands.push_back({
				.buffer_idx     = buffer_idx,
				.vertex_count   = vertex_count,
				.instance_count = instance_count,
				.vertex_offset  = vertex_offset,
				.index_offset   = index_offset,
			});
		}

		void draw_frame()
		{
			auto result                    = vk::Result{};
			auto image_index               = 0u;
			auto in_flight_fence           = sync_objects.at(current_frame).in_flight;
			auto image_available_semaphore = sync_objects.at(current_frame).image_available;
			auto render_finished_semaphore = sync_objects.at(current_frame).render_finished;
			auto command_buffer            = command_buffers.at(current_frame);
			auto sc                        = vk_swap_chain->get();
			auto dev                       = vk_device->get_device();

			result = dev.waitForFences(in_flight_fence, true, 1'000'000'000);

			std::tie(result, image_index) = dev.acquireNextImageKHR(sc, 1'000'000'000, image_available_semaphore, VK_NULL_HANDLE);
			resize_check(result, 1);

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
			resize_check(result, 2);

			current_frame = (current_frame + 1) % max_frames_in_flight; // 0 ... max_frame
		}

		void window_resized(HWND window_handle)
		{
			std::println("Window Resized event triggered.");

			// Wait till gpu is idle
			vk_device->get_device().waitIdle();

			// destroy semaphores, fences, command_buffers and swapchain
			destroy_synchronization_objects();
			command_buffers.clear();
			vk_swap_chain.reset();

			vk_instance->update_surface(window_handle);
			vk_swap_chain = std::make_unique<swap_chain>(vk_instance.get(), vk_device.get());
			// recreate command buffers and sync objects
			create_command_buffers();
			create_synchronization_objects();
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
			sync_objects.resize(max_frames_in_flight);

			for (auto &&[image_available_semaphore, render_finished_semaphore, in_flight_fence] : sync_objects)
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

			for (auto &&[image_available_semaphore, render_finished_semaphore, in_flight_fence] : sync_objects)
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
				cb.bindPipeline(vk::PipelineBindPoint::eGraphics, gfx_pipeline->get_pipeline());

				auto viewport = vk::Viewport{
					.x        = 0.0f,
					.y        = static_cast<float>(extent.height), // Top of the viewport is bottom of the screen to get +Y to point up
					.width    = static_cast<float>(extent.width),
					.height   = static_cast<float>(extent.height) * -1.f, // make height negative so +Y points up
					.minDepth = 0.0f,
					.maxDepth = 1.0f
				};
				cb.setViewport(0, viewport);

				auto scissor = vk::Rect2D{
					.offset = { 0, 0 },
					.extent = extent
				};
				cb.setScissor(0, scissor);

				for (auto &vb : draw_vb_commands)
				{
					cb.bindVertexBuffers(0, // first binding
					                     gfx_buffers.at(vb.buffer_idx).get_buffer(),
					                     { 0 });
					cb.draw(vb.vertex_count, vb.instance_count, vb.vertex_offset, vb.index_offset);
				}
			}
			cb.endRenderPass();
		}

		void resize_check(vk::Result result, uint8_t result_point)
		{
			switch (result)
			{
				using enum vk::Result;
			case eSuccess:
				break;
			case eErrorOutOfDateKHR:
				// Resize SurfaceKHR
				std::println("{} Resize?: {}", result_point, vk::to_string(result));
				break;
			case eSuboptimalKHR:
				// Resize Swapchain
				std::println("{} Resize?: {}", result_point, vk::to_string(result));
				break;
			case eErrorDeviceLost:
				// Rebuild SurfaceKHR
				std::println("{} Resize?: {}", result_point, vk::to_string(result));
				break;
			default:
				std::println("{} Unknown Error: {}", result_point, vk::to_string(result));
				break;
			}
		}

	private:
		std::unique_ptr<instance> vk_instance     = nullptr;
		std::unique_ptr<device> vk_device         = nullptr;
		std::unique_ptr<swap_chain> vk_swap_chain = nullptr;

		uint32_t max_frames_in_flight = 0;
		uint32_t current_frame        = 0;

		vk::CommandPool command_pool;
		std::vector<vk::CommandBuffer> command_buffers;

		struct frame_sync_data
		{
			vk::Semaphore image_available;
			vk::Semaphore render_finished;
			vk::Fence in_flight;
		};
		std::vector<frame_sync_data> sync_objects;

		std::array<float, 4> clear_color = { 0.2f, 0.2f, 0.4f, 1.f };

		std::unique_ptr<pipeline> gfx_pipeline = nullptr;

		std::vector<buffer> gfx_buffers;

		struct draw_vb_data
		{
			uint32_t buffer_idx;
			uint32_t vertex_count;
			uint32_t instance_count;
			uint32_t vertex_offset;
			uint32_t index_offset;
		};
		std::vector<draw_vb_data> draw_vb_commands;
	};
}
