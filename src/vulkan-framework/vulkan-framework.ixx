module;

#include <cassert>
#include "vma-usage.hpp"

export module vfw;

import std;

import :context;
import :swapchain;
import :commandpool;
import :synchronization;
import :types;

export namespace vfw
{
	class renderer final
	{
	public:
		renderer(HWND hWnd)
		{
			auto get_window_size = [](HWND window_handle) {
				RECT rect{};
				GetClientRect(window_handle, &rect);

				return std::array{
					static_cast<std::uint16_t>(rect.right - rect.left),
					static_cast<std::uint16_t>(rect.bottom - rect.top)
				};
			};

			auto [width, height] = get_window_size(hWnd);

			ctx = std::make_unique<context>(hWnd);
			sc  = std::make_unique<swapchain>(ctx->get_device(), ctx->get_mem_allocator(),
			                                  swapchain::description{
												  .width      = width,
												  .height     = height,
												  .surface    = ctx->get_surface(),
												  .chosen_gpu = ctx->get_chosen_gpu(),
                                             });

			max_frame_count = sc->get_image_count();

			cp = std::make_unique<commandpool>(ctx->get_device(),
			                                   commandpool::description{
												   .max_frame_count      = max_frame_count,
												   .graphics_queue       = ctx->get_graphics_queue(),
												   .graphics_queue_index = ctx->get_graphics_queue_family(),
											   });

			fs = std::make_unique<frame_sync>(ctx->get_device(), max_frame_count);
		}

		~renderer() = default;

		void window_resized(uint16_t width, uint16_t height)
		{
			ctx->window_resized(width, height);

			sc.reset(nullptr);
			sc = std::make_unique<swapchain>(
				ctx->get_device(),
				ctx->get_mem_allocator(),
				swapchain::description{
					.width      = width,
					.height     = height,
					.surface    = ctx->get_surface(),
					.chosen_gpu = ctx->get_chosen_gpu(),
				});

			// TODO: would frame count changed because of window resize??
			// should never happen.
			if (max_frame_count not_eq sc->get_image_count())
			{
				max_frame_count = sc->get_image_count();
				cp.reset(nullptr);
				cp = std::make_unique<commandpool>(
					ctx->get_device(),
					commandpool::description{
						.max_frame_count      = max_frame_count,
						.graphics_queue       = ctx->get_graphics_queue(),
						.graphics_queue_index = ctx->get_graphics_queue_family(),
					});
			}
		}

		void draw()
		{
			const auto wait_time = 1'000'000'000u;
			auto device          = ctx->get_device();
			auto fs_sf           = fs->get_sync_objects_at(current_frame);
			auto cb              = cp->get_buffer_at(current_frame);

			auto result = device.waitForFences(fs_sf.render_fence, true, wait_time);
			assert(result == vk::Result::eSuccess);
			device.resetFences(fs_sf.render_fence);

			auto image_index = sc->next_image_index(fs_sf.swapchain_semaphore);
			assert(image_index <= max_frame_count);

			record_command_buffer(cb, image_index);

			submit_command_buffer(cb, fs_sf);

			ctx->present(sc->get_swapchain(), image_index, fs_sf.render_semaphore);

			current_frame = (current_frame + 1) % max_frame_count; // 0...max_frame_count
		}

	private:
		void record_command_buffer(vk::CommandBuffer &cb, uint32_t image_index)
		{
			cb.reset();
			auto cb_bi  = vk::CommandBufferBeginInfo{};
			auto result = cb.begin(&cb_bi);
			assert(result == vk::Result::eSuccess);

			sc->transition_image(cb, image_index, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);

			auto clear_color = std::array{ 0.4f, 0.5f, 0.4f, 1.0f };
			auto clear_value = vk::ClearValue{
				.color = clear_color,
			};

			sc->clear_image(cb, image_index, vk::ImageLayout::eGeneral, clear_value, vk::ImageAspectFlagBits::eColor);

			sc->transition_image(cb, image_index, vk::ImageLayout::eGeneral, vk::ImageLayout::ePresentSrcKHR);

			cb.end();
		}

		void submit_command_buffer(vk::CommandBuffer &cb, const frame_sync::sync_flags &fs_sf)
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

			auto graphics_queue = ctx->get_graphics_queue();
			graphics_queue.submit2(submit_info, fs_sf.render_fence);
		}

	private:
		std::unique_ptr<context> ctx{ nullptr };
		std::unique_ptr<swapchain> sc{ nullptr };
		std::unique_ptr<commandpool> cp{ nullptr };
		std::unique_ptr<frame_sync> fs{ nullptr };

		uint32_t max_frame_count = 0;
		uint32_t current_frame   = 0;
	};
}