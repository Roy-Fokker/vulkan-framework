module;

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
			auto [width, height] = get_window_size(hWnd);

			ctx = std::make_unique<context>(hWnd);
			sc  = std::make_unique<swapchain>(
                ctx->get_device(),
                ctx->get_mem_allocator(),
                swapchain::description{
					 .width      = width,
					 .height     = height,
					 .surface    = ctx->get_surface(),
					 .chosen_gpu = ctx->get_chosen_gpu(),
                });

			max_frame_count = sc->get_image_count();

			cp = std::make_unique<commandpool>(
				ctx->get_device(),
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

	private:
		auto get_window_size(HWND window_handle) -> const std::array<uint16_t, 2>
		{
			RECT rect{};
			GetClientRect(window_handle, &rect);

			return {
				static_cast<std::uint16_t>(rect.right - rect.left),
				static_cast<std::uint16_t>(rect.bottom - rect.top)
			};
		}

		void create_sync_objects()
		{
			auto device = ctx->get_device();
			sync_objects.resize(max_frame_count);

			for (auto &&sync_object : sync_objects)
			{
				auto semaphore_ci               = vk::SemaphoreCreateInfo{};
				sync_object.swapchain_semaphore = device.createSemaphore(semaphore_ci);
				sync_object.render_semaphore    = device.createSemaphore(semaphore_ci);

				auto fence_ci = vk::FenceCreateInfo{
					.flags = vk::FenceCreateFlagBits::eSignaled
				};

				sync_object.render_fence = device.createFence(fence_ci);
			}
		}

		void destroy_sync_objects()
		{
			auto device = ctx->get_device();

			device.waitIdle();

			for (auto &&sync_object : sync_objects)
			{
				device.destroyFence(sync_object.render_fence);
				device.destroySemaphore(sync_object.render_semaphore);
				device.destroySemaphore(sync_object.swapchain_semaphore);
			}
		}

	private:
		std::unique_ptr<context> ctx{ nullptr };
		std::unique_ptr<swapchain> sc{ nullptr };
		std::unique_ptr<commandpool> cp{ nullptr };
		std::unique_ptr<frame_sync> fs{ nullptr };

		std::vector<types::frame_sync> sync_objects{};

		uint16_t max_frame_count = 0;
	};
}