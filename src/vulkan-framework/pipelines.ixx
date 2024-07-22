module;

#include <cassert>

export module vfw:pipelines;

import std;
import :types;

export namespace vfw
{
	auto make_shader_module(vk::Device device, std::span<uint32_t> spv_bin) -> vk::ShaderModule
	{
		auto ci = vk::ShaderModuleCreateInfo{
			.codeSize = spv_bin.size(),
			.pCode    = spv_bin.data(),
		};

		return device.createShaderModule(ci);
	}

	class compute_pipeline final
	{
	public:
		compute_pipeline()                                       = delete;
		compute_pipeline(const compute_pipeline &src)            = delete;
		compute_pipeline &operator=(const compute_pipeline &src) = delete;
		compute_pipeline(compute_pipeline &&src)                 = delete;
		compute_pipeline &operator=(compute_pipeline &&src)      = delete;

		compute_pipeline(vk::Device device, vk::DescriptorSetLayout &set_layout)
			: device(device)
		{
			auto pc_rng = vk::PushConstantRange{
				.stageFlags = vk::ShaderStageFlagBits::eCompute,
				.offset     = 0,
				.size       = sizeof(types::compute_push_constants),
			};

			auto pl_ci = vk::PipelineLayoutCreateInfo{
				.setLayoutCount         = 1,
				.pSetLayouts            = &set_layout,
				.pushConstantRangeCount = 1,
				.pPushConstantRanges    = &pc_rng,
			};

			layout = device.createPipelineLayout(pl_ci);
		}

		~compute_pipeline()
		{
			device.waitIdle();

			device.destroyPipeline(pl);
			device.destroyPipelineLayout(layout);
		}

		void add_shader(types::shader_stage stage, std::span<uint32_t> data)
		{
			auto shader_module = make_shader_module(device, data);
			auto stage_info    = vk::PipelineShaderStageCreateInfo{
				   .stage  = types::to_vk_flag(stage),
				   .module = shader_module,
				   .pName  = "main",
			};

			if (stage == types::shader_stage::compute)
			{
				auto cp_ci = vk::ComputePipelineCreateInfo{
					.stage  = stage_info,
					.layout = layout,
				};

				auto result = device.createComputePipeline(VK_NULL_HANDLE, cp_ci);
				pl          = result.value;
			}
			else
			{
				assert(false);
			}

			device.destroyShaderModule(shader_module);
		}

		auto get_pipeline() -> vk::Pipeline
		{
			return pl;
		}

		auto get_layout() -> vk::PipelineLayout
		{
			return layout;
		}

	private:
		vk::Device device;

		vk::PipelineLayout layout;
		vk::Pipeline pl;
	};

	class graphics_pipeline final
	{
	public:
		graphics_pipeline()                                        = delete;
		graphics_pipeline(const graphics_pipeline &src)            = delete;
		graphics_pipeline &operator=(const graphics_pipeline &src) = delete;
		graphics_pipeline(graphics_pipeline &&src)                 = delete;
		graphics_pipeline &operator=(graphics_pipeline &&src)      = delete;

		graphics_pipeline(vk::Device device)
			: device(device)
		{
		}

		~graphics_pipeline()
		{
			device.waitIdle();

			device.destroyPipeline(pipeline);
			device.destroyPipelineLayout(layout);
		}

	private:
		vk::Device device;
		vk::PipelineLayout layout;
		vk::Pipeline pipeline;

		friend struct graphics_pipeline_builder;
	};

	struct graphics_pipeline_builder final
	{
		std::vector<vk::PipelineShaderStageCreateInfo> shader_stages{};
		vk::PipelineInputAssemblyStateCreateInfo input_assembly{};
		vk::PipelineRasterizationStateCreateInfo rasterizer{};
		vk::PipelineColorBlendAttachmentState color_blend_attachment{};
		vk::PipelineMultisampleStateCreateInfo multisampling{};
		vk::PipelineDepthStencilStateCreateInfo depth_stencil{};
		vk::Format color_attachment_format{};
		vk::PipelineRenderingCreateInfo render_info{};
		vk::PipelineLayout pipeline_layout{};

		graphics_pipeline_builder() = default;

		[[nodiscard]]
		auto build(vk::Device device) -> std::unique_ptr<graphics_pipeline> // vk::Pipeline
		{
			auto viewport_state = vk::PipelineViewportStateCreateInfo{
				.viewportCount = 1,
				.scissorCount  = 1,
			};

			auto color_blending = vk::PipelineColorBlendStateCreateInfo{
				.logicOpEnable   = vk::False,
				.logicOp         = vk::LogicOp::eCopy,
				.attachmentCount = 1,
				.pAttachments    = &color_blend_attachment,
			};

			auto vertex_inputs = vk::PipelineVertexInputStateCreateInfo{};

			auto dynamic_states = std::array{
				vk::DynamicState::eViewport,
				vk::DynamicState::eScissor,
			};

			auto dynamic_state_ci = vk::PipelineDynamicStateCreateInfo{
				.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
				.pDynamicStates    = dynamic_states.data(),
			};

			auto pipeline_ci = vk::GraphicsPipelineCreateInfo{
				.pNext               = &render_info,
				.stageCount          = static_cast<uint32_t>(shader_stages.size()),
				.pStages             = shader_stages.data(),
				.pVertexInputState   = &vertex_inputs,
				.pInputAssemblyState = &input_assembly,
				.pViewportState      = &viewport_state,
				.pRasterizationState = &rasterizer,
				.pMultisampleState   = &multisampling,
				.pDepthStencilState  = &depth_stencil,
				.pColorBlendState    = &color_blending,
				.pDynamicState       = &dynamic_state_ci,
				.layout              = pipeline_layout,
			};

			auto result_value = device.createGraphicsPipeline(nullptr, pipeline_ci);

			assert(result_value.result == vk::Result::eSuccess);

			auto pl      = std::make_unique<graphics_pipeline>(device);
			pl->layout   = pipeline_layout;
			pl->pipeline = result_value.value;

			return std::move(pl);
		}

		void set_shaders(std::span<types::shader_module> shaders)
		{

			auto make_ci = [&](auto &&sm) {
				return vk::PipelineShaderStageCreateInfo{
					.stage  = types::to_vk_flag(sm.stage),
					.module = sm.shdr,
					.pName  = "main",
				};
			};

			shader_stages = shaders | std::views::transform(make_ci) | std::ranges::to<std::vector>();
		}

		void set_input_topology(vk::PrimitiveTopology topology)
		{
			input_assembly = vk::PipelineInputAssemblyStateCreateInfo{
				.topology               = topology,
				.primitiveRestartEnable = vk::False,
			};
		}

		// TODO: merge rasterizer impacted methods
		void set_polygon_mode(vk::PolygonMode mode)
		{
			rasterizer.polygonMode = mode;
			rasterizer.lineWidth   = 1.f;
		}

		void set_cull_mode(vk::CullModeFlags cull_mode, vk::FrontFace front_face)
		{
			rasterizer.cullMode  = cull_mode;
			rasterizer.frontFace = front_face;
		}

		void set_multisampling_none()
		{
			multisampling = vk::PipelineMultisampleStateCreateInfo{
				.rasterizationSamples  = vk::SampleCountFlagBits::e1,
				.sampleShadingEnable   = vk::False,
				.minSampleShading      = 1.0f,
				.alphaToCoverageEnable = vk::False,
				.alphaToOneEnable      = vk::False,
			};
		}

		void set_blending_mode(types::blending_mode mode)
		{
			switch (mode)
			{
				using enum types::blending_mode;
			case none:
				color_blend_attachment.blendEnable = vk::False;
				break;
			case additive:
				color_blend_attachment = vk::PipelineColorBlendAttachmentState{
					.blendEnable = vk::True,

					.srcColorBlendFactor = vk::BlendFactor::eOne,
					.dstColorBlendFactor = vk::BlendFactor::eDstAlpha,
					.colorBlendOp        = vk::BlendOp::eAdd,

					.srcAlphaBlendFactor = vk::BlendFactor::eOne,
					.dstAlphaBlendFactor = vk::BlendFactor::eZero,
					.alphaBlendOp        = vk::BlendOp::eAdd,
				};
				break;
			case alpha_blend:
				color_blend_attachment = vk::PipelineColorBlendAttachmentState{
					.blendEnable = vk::True,

					.srcColorBlendFactor = vk::BlendFactor::eOneMinusDstAlpha,
					.dstColorBlendFactor = vk::BlendFactor::eDstAlpha,
					.colorBlendOp        = vk::BlendOp::eAdd,

					.srcAlphaBlendFactor = vk::BlendFactor::eOne,
					.dstAlphaBlendFactor = vk::BlendFactor::eZero,
					.alphaBlendOp        = vk::BlendOp::eAdd,
				};
				break;
			}

			color_blend_attachment.colorWriteMask = vk::ColorComponentFlagBits::eR |
			                                        vk::ColorComponentFlagBits::eG |
			                                        vk::ColorComponentFlagBits::eB |
			                                        vk::ColorComponentFlagBits::eA;
		}

		void set_color_attachment_format(vk::Format format)
		{
			color_attachment_format             = format;
			render_info.colorAttachmentCount    = 1;
			render_info.pColorAttachmentFormats = &color_attachment_format;
		}

		void set_depth_format(vk::Format format)
		{
			render_info.depthAttachmentFormat = format;
		}

		void disable_depthtest()
		{
			depth_stencil = vk::PipelineDepthStencilStateCreateInfo{
				.depthTestEnable       = vk::False,
				.depthWriteEnable      = vk::False,
				.depthCompareOp        = vk::CompareOp::eNever,
				.depthBoundsTestEnable = vk::False,
				.stencilTestEnable     = vk::False,
				.minDepthBounds        = 0.f,
				.maxDepthBounds        = 1.f,
			};
		}

		void enable_depthtest(bool enable_depth_write, vk::CompareOp op)
		{
			depth_stencil = vk::PipelineDepthStencilStateCreateInfo{
				.depthTestEnable       = vk::True,
				.depthWriteEnable      = enable_depth_write,
				.depthCompareOp        = op,
				.depthBoundsTestEnable = vk::False,
				.stencilTestEnable     = vk::False,
				.minDepthBounds        = 0.f,
				.maxDepthBounds        = 1.f,
			};
		}
	};
}