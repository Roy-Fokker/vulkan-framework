module;

export module vfw:swap_chain;

import :instance;
import :device;

export namespace vfw
{
	class swap_chain
	{
	public:
		explicit swap_chain(const instance *vfw_inst, const device *vfw_device)
		{
			vk_device            = vfw_device->get_device();
			auto physical_device = vfw_device->get_physical_device();
			auto qf              = vfw_device->get_queue_family();
			auto surface         = vfw_inst->get_surface();
			auto sd              = vfw_inst->query_surface_details(physical_device);

			create_swap_chain(surface, qf, sd);
			create_images();
			create_renderpass();
			create_frame_buffers();
		}

		~swap_chain()
		{

			destroy_frame_buffers();
			destroy_images();
			vk_device.destroyRenderPass(vk_render_pass);
			vk_device.destroySwapchainKHR(vk_swap_chain);
		}

		swap_chain() = delete;

		[[nodiscard]] auto get() const -> const vk::SwapchainKHR &
		{
			return vk_swap_chain;
		}

		[[nodiscard]] auto get_render_pass() const -> const vk::RenderPass &
		{
			return vk_render_pass;
		}

		[[nodiscard]] auto get_extent() const -> const vk::Extent2D
		{
			return vk_sc_extent;
		}

		[[nodiscard]] auto frame_buffer(uint32_t index) const -> const vk::Framebuffer &
		{
			return vk_frame_buffers.at(index);
		}

	private:
		void create_swap_chain(const vk::SurfaceKHR &surface, const queue_family &qf, const surface_details &sd);
		void create_images()
		{
			vk_images = vk_device.getSwapchainImagesKHR(vk_swap_chain);
			vk_image_views.resize(vk_images.size());

			for (auto &&[image, image_view] : std::views::zip(vk_images, vk_image_views))
			{
				auto create_info = vk::ImageViewCreateInfo{
					.image      = image,
					.viewType   = vk::ImageViewType::e2D,
					.format     = vk_sc_format,
					.components = {
						.r = vk::ComponentSwizzle::eIdentity,
						.g = vk::ComponentSwizzle::eIdentity,
						.b = vk::ComponentSwizzle::eIdentity,
						.a = vk::ComponentSwizzle::eIdentity,
					},
					.subresourceRange = { .aspectMask = vk::ImageAspectFlagBits::eColor, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1 },
				};

				image_view = vk_device.createImageView(create_info);
			}
		}

		void create_renderpass()
		{
			auto color_attachment = vk::AttachmentDescription{
				.format         = vk_sc_format,
				.samples        = vk::SampleCountFlagBits::e1,
				.loadOp         = vk::AttachmentLoadOp::eClear,
				.storeOp        = vk::AttachmentStoreOp::eStore,
				.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare,
				.stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
				.initialLayout  = vk::ImageLayout::eUndefined,
				.finalLayout    = vk::ImageLayout::ePresentSrcKHR
			};

			auto color_attachment_ref = vk::AttachmentReference{
				.attachment = 0,
				.layout     = vk::ImageLayout::eColorAttachmentOptimal
			};

			auto sub_pass = vk::SubpassDescription{
				.pipelineBindPoint    = vk::PipelineBindPoint::eGraphics,
				.colorAttachmentCount = 1,
				.pColorAttachments    = &color_attachment_ref
			};

			auto dependency = vk::SubpassDependency{
				.srcSubpass    = VK_SUBPASS_EXTERNAL,
				.dstSubpass    = 0,
				.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput,
				.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput,
				.srcAccessMask = vk::AccessFlagBits::eNone,
				.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite
			};

			auto create_info = vk::RenderPassCreateInfo{
				.attachmentCount = 1,
				.pAttachments    = &color_attachment,
				.subpassCount    = 1,
				.pSubpasses      = &sub_pass,
				.dependencyCount = 1,
				.pDependencies   = &dependency
			};

			vk_render_pass = vk_device.createRenderPass(create_info);
		}

		void create_frame_buffers()
		{
			vk_frame_buffers.resize(vk_image_views.size());

			for (auto &&[image_view, frame_buffer] : std::views::zip(vk_image_views, vk_frame_buffers))
			{
				auto attachments = std::vector<vk::ImageView>{ image_view };
				auto create_info = vk::FramebufferCreateInfo{
					.renderPass      = vk_render_pass,
					.attachmentCount = static_cast<uint32_t>(attachments.size()),
					.pAttachments    = attachments.data(),
					.width           = vk_sc_extent.width,
					.height          = vk_sc_extent.height,
					.layers          = 1
				};

				frame_buffer = vk_device.createFramebuffer(create_info);
			}
		}

		void destroy_images()
		{
			for (auto &&[image, image_view] : std::views::zip(vk_images, vk_image_views))
			{
				if (image_view)
				{
					vk_device.destroyImageView(image_view);
					image_view = nullptr;
				}
			}
		}

		void destroy_frame_buffers()
		{
			for (auto &fb : vk_frame_buffers)
			{
				if (fb)
				{
					vk_device.destroyFramebuffer(fb);
					fb = nullptr;
				}
			}
		}

	private:
		vk::Device vk_device;
		vk::SwapchainKHR vk_swap_chain;
		vk::Format vk_sc_format;
		vk::Extent2D vk_sc_extent;
		vk::RenderPass vk_render_pass;
		std::vector<vk::Image> vk_images;
		std::vector<vk::ImageView> vk_image_views;
		std::vector<vk::Framebuffer> vk_frame_buffers;
	};
}

namespace
{
	auto pick_surface_format(const surface_details &sd) -> vk::SurfaceFormatKHR
	{
		using eFmt = vk::Format;
		using eCS  = vk::ColorSpaceKHR;

		auto format_iter = std::ranges::find_if(sd.formats, [](const vk::SurfaceFormatKHR &sf) {
			return sf.format == eFmt::eB8G8R8A8Srgb and sf.colorSpace == eCS::eSrgbNonlinear;
		});

		if (format_iter == sd.formats.end())
		{
			throw std::runtime_error("Unable to find desired surface format");
		}

		return *format_iter;
	}

	auto pick_present_mode(const surface_details &sd) -> vk::PresentModeKHR
	{
		auto mode_iter = std::ranges::find_if(sd.present_modes, [](const vk::PresentModeKHR &pm) {
			return pm == vk::PresentModeKHR::eFifoRelaxed;
		});

		if (mode_iter == sd.present_modes.end())
		{
			throw std::runtime_error("Unable to find desired surface format");
		}

		return *mode_iter;
	}

	auto pick_surface_extent(const surface_details &sd) -> vk::Extent2D
	{
		if (sd.capabilities.currentExtent.width == std::numeric_limits<uint32_t>::max())
		{
			throw std::runtime_error("Current extent width exceeds numeric max.");
		}

		return sd.capabilities.currentExtent;
	}
}

using namespace vfw;

void swap_chain::create_swap_chain(const vk::SurfaceKHR &surface, const queue_family &qf, const surface_details &sd)
{
	auto sf      = pick_surface_format(sd);
	auto pm      = pick_present_mode(sd);
	vk_sc_extent = pick_surface_extent(sd);
	vk_sc_format = sf.format;

	auto image_count = std::clamp(0u, sd.capabilities.minImageCount + 1, sd.capabilities.maxImageCount);
	auto ism         = (qf.graphics_family == qf.present_family) ? vk::SharingMode::eExclusive
	                                                             : vk::SharingMode::eConcurrent;
	auto qfl         = (qf.graphics_family == qf.present_family) ? std::vector<uint32_t>{}
	                                                             : std::vector{ qf.graphics_family.value(), qf.present_family.value() };

	auto create_info = vk::SwapchainCreateInfoKHR{
		.surface               = surface,
		.minImageCount         = image_count,
		.imageFormat           = sf.format,
		.imageColorSpace       = sf.colorSpace,
		.imageExtent           = vk_sc_extent,
		.imageArrayLayers      = 1,
		.imageUsage            = vk::ImageUsageFlagBits::eColorAttachment,
		.imageSharingMode      = ism,
		.queueFamilyIndexCount = static_cast<uint32_t>(qfl.size()),
		.pQueueFamilyIndices   = qfl.data(),
		.preTransform          = sd.capabilities.currentTransform,
		.compositeAlpha        = vk::CompositeAlphaFlagBitsKHR::eOpaque,
		.presentMode           = pm,
		.clipped               = true
	};

	vk_swap_chain = vk_device.createSwapchainKHR(create_info);
}