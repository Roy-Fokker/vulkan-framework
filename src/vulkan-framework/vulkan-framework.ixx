module;

#include "vma-usage.hpp"

export module vfw;

import std;

import :context;
import :swapchain;

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

	private:
		std::unique_ptr<context> ctx{ nullptr };
		std::unique_ptr<swapchain> sc{ nullptr };
	};
}