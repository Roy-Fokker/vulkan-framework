module;

#include <cassert>
#include <VkBootstrap.h>
#include <vma/vk_mem_alloc.h>

export module vfw:image;

import std;

export namespace vfw
{
	class allocated_image final
	{
	public:
		struct description
		{
			uint16_t width;
			uint16_t height;
			vk::Format format;
		};

	public:
		allocated_image()                                      = delete;
		allocated_image(const allocated_image &src)            = delete;
		allocated_image &operator=(const allocated_image &src) = delete;
		allocated_image(allocated_image &&src)                 = delete;
		allocated_image &operator=(allocated_image &&src)      = delete;

		allocated_image(vk::Device device, VmaAllocator allocator, const description &desc)
			: device{ device }, allocator{ allocator },
			  format{ desc.format }, extent{ desc.width, desc.height, 1 }
		{
			allocate_image_memory();
		}

		~allocated_image()
		{
			free_image_memory();
		}

		[[nodiscard]] auto get_image() -> vk::Image &
		{
			return vkimage;
		}

		[[nodiscard]] auto get_view() -> vk::ImageView
		{
			return vkview;
		}

		[[nodiscard]] auto get_size() -> vk::Extent2D
		{
			return {
				extent.width,
				extent.height,
			};
		}

	private:
		void allocate_image_memory()
		{
			auto draw_image_usage = vk::ImageUsageFlags{
				vk::ImageUsageFlagBits::eTransferSrc |
				vk::ImageUsageFlagBits::eTransferDst |
				vk::ImageUsageFlagBits::eStorage |
				vk::ImageUsageFlagBits::eColorAttachment
			};

			auto image_ci = vk::ImageCreateInfo{
				.imageType   = vk::ImageType::e2D,
				.format      = format,
				.extent      = extent,
				.mipLevels   = 1,
				.arrayLayers = 1,
				.samples     = vk::SampleCountFlagBits::e1,
				.tiling      = vk::ImageTiling::eOptimal,
				.usage       = draw_image_usage,
			};

			auto image_ai = VmaAllocationCreateInfo{
				.usage         = VMA_MEMORY_USAGE_GPU_ONLY,
				.requiredFlags = VkMemoryPropertyFlags{ VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT },
			};

			vmaCreateImage(allocator,
			               reinterpret_cast<VkImageCreateInfo *>(&image_ci),
			               &image_ai,
			               reinterpret_cast<VkImage *>(&vkimage),
			               &allocation,
			               nullptr);

			auto view_ci = vk::ImageViewCreateInfo{
				.image            = vkimage,
				.viewType         = vk::ImageViewType::e2D,
				.format           = format,
				.subresourceRange = {
					.aspectMask     = vk::ImageAspectFlagBits::eColor,
					.baseMipLevel   = 0,
					.levelCount     = 1,
					.baseArrayLayer = 0,
					.layerCount     = 1,
				},
			};

			vkview = device.createImageView(view_ci);
		}

		void free_image_memory()
		{
			device.waitIdle();

			device.destroyImageView(vkview);
			vmaDestroyImage(allocator, vkimage, allocation);
		}

	private:
		vk::Device device;
		VmaAllocator allocator;

		vk::Image vkimage;
		vk::ImageView vkview;
		VmaAllocation allocation;
		vk::Extent3D extent;
		vk::Format format;
	};

	void transition_image(vk::CommandBuffer &cb, vk::Image &image, vk::ImageLayout current_layout, vk::ImageLayout new_layout)
	{
		auto aspect_mask = vk::ImageAspectFlags{};
		if (new_layout == vk::ImageLayout::eDepthAttachmentOptimal)
		{
			aspect_mask = vk::ImageAspectFlagBits::eDepth;
		}
		else
		{
			aspect_mask = vk::ImageAspectFlagBits::eColor;
		}

		auto image_barrier = vk::ImageMemoryBarrier2{
			.srcStageMask  = vk::PipelineStageFlagBits2::eAllCommands,
			.srcAccessMask = vk::AccessFlagBits2::eMemoryWrite,
			.dstStageMask  = vk::PipelineStageFlagBits2::eAllCommands,
			.dstAccessMask = vk::AccessFlagBits2::eMemoryWrite | vk::AccessFlagBits2::eMemoryRead,

			.oldLayout = current_layout,
			.newLayout = new_layout,

			.image            = image,
			.subresourceRange = {
				.aspectMask     = aspect_mask,
				.baseMipLevel   = 0,
				.levelCount     = vk::RemainingMipLevels,
				.baseArrayLayer = 0,
				.layerCount     = vk::RemainingArrayLayers,
			},
		};

		auto dep_info = vk::DependencyInfo{
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers    = &image_barrier,
		};

		cb.pipelineBarrier2(&dep_info);
	}

	void copy_image(vk::CommandBuffer &cb, vk::Image &src, vk::Extent2D &src_size, vk::Image &dst, vk::Extent2D &dst_size)
	{
		auto src_offset = std::array<vk::Offset3D, 2>{};
		src_offset[1]   = {
            // TODO: why is it index 1, is because we don't need to say index 0 is 0, 0???
			  .x = (int32_t)src_size.width,
			  .y = (int32_t)src_size.height,
			  .z = 1,
		};
		auto dst_offset = std::array<vk::Offset3D, 2>{};
		dst_offset[1]   = {
            // TODO: why is it index 1, is because we don't need to say index 0 is 0, 0???
			  .x = (int32_t)dst_size.width,
			  .y = (int32_t)dst_size.height,
			  .z = 1,
		};

		auto blit_region = vk::ImageBlit2{
			.srcSubresource = {
				.aspectMask     = vk::ImageAspectFlagBits::eColor,
				.mipLevel       = 0,
				.baseArrayLayer = 0,
				.layerCount     = 1,
			},
			.srcOffsets     = src_offset,
			.dstSubresource = {
				.aspectMask     = vk::ImageAspectFlagBits::eColor,
				.mipLevel       = 0,
				.baseArrayLayer = 0,
				.layerCount     = 1,
			},
			.dstOffsets = dst_offset,
		};

		auto blit_info = vk::BlitImageInfo2{
			.srcImage       = src,
			.srcImageLayout = vk::ImageLayout::eTransferSrcOptimal,
			.dstImage       = dst,
			.dstImageLayout = vk::ImageLayout::eTransferDstOptimal,
			.regionCount    = 1,
			.pRegions       = &blit_region,
			.filter         = vk::Filter::eLinear,
		};

		cb.blitImage2(&blit_info);
	}

	void clear_image(vk::CommandBuffer &cb, vk::Image &image, const vk::ClearValue &clear_value)
	{
		auto clear_range = vk::ImageSubresourceRange{
			.aspectMask     = vk::ImageAspectFlagBits::eColor,
			.baseMipLevel   = 0,
			.levelCount     = vk::RemainingMipLevels,
			.baseArrayLayer = 0,
			.layerCount     = vk::RemainingArrayLayers,
		};

		cb.clearColorImage(image,
		                   vk::ImageLayout::eGeneral,
		                   clear_value.color,
		                   clear_range);
	}
}