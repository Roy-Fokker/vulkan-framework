module;

#include <cassert>
#include "vma-usage.hpp"

export module vfw;

import std;

import :context;
import :swapchain;
import :commandpool;
import :synchronization;
import :image;
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
			sc  = std::make_unique<swapchain>(ctx->get_device(),
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

			rndr_img = std::make_unique<allocated_image>(ctx->get_device(), ctx->get_mem_allocator(),
			                                             allocated_image::description{
															 .width  = width,
															 .height = height,
															 .format = vk::Format::eR16G16B16A16Sfloat,
														 });
		}

		~renderer() = default;

		void window_resized(uint16_t width, uint16_t height)
		{
			ctx->window_resized(width, height);

			sc.reset(nullptr);
			sc = std::make_unique<swapchain>(
				ctx->get_device(),
				swapchain::description{
					.width      = width,
					.height     = height,
					.surface    = ctx->get_surface(),
					.chosen_gpu = ctx->get_chosen_gpu(),
				});

			rndr_img.reset(nullptr);
			rndr_img = std::make_unique<allocated_image>(ctx->get_device(), ctx->get_mem_allocator(),
			                                             allocated_image::description{
															 .width  = width,
															 .height = height,
															 .format = vk::Format::eR16G16B16A16Sfloat,
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

			ctx->submit(cb, fs_sf);
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

			auto image    = rndr_img->get_image();
			auto img_size = rndr_img->get_size();

			transition_image(cb, image, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral);
			draw_on_image(cb, image);
			transition_image(cb, image, vk::ImageLayout::eGeneral, vk::ImageLayout::eTransferSrcOptimal);

			auto sc_image = sc->get_image(image_index);
			auto sc_size  = sc->get_size();

			transition_image(cb, sc_image, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
			copy_image(cb, image, img_size, sc_image, sc_size);
			transition_image(cb, sc_image, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR);

			cb.end();
		}

		void draw_on_image(vk::CommandBuffer &cb, vk::Image &image)
		{
			auto clear_color = std::array{ 0.4f, 0.5f, 0.4f, 1.0f };
			auto clear_value = vk::ClearValue{
				.color = clear_color,
			};

			clear_image(cb, image, clear_value);
		}

	private:
		std::unique_ptr<context> ctx{ nullptr };
		std::unique_ptr<swapchain> sc{ nullptr };
		std::unique_ptr<commandpool> cp{ nullptr };
		std::unique_ptr<frame_sync> fs{ nullptr };

		std::unique_ptr<allocated_image> rndr_img{ nullptr };

		uint32_t max_frame_count = 0;
		uint32_t current_frame   = 0;
	};
}