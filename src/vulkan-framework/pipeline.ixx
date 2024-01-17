module;

export module vfw:pipeline;

export namespace vfw
{
	class pipeline
	{
	public:
		struct description
		{
			using shader_binary     = std::vector<uint32_t> &;
			using shader_stage_file = std::tuple<vk::ShaderStageFlagBits, shader_binary>;

			std::vector<shader_stage_file> shaders; // Note: Do not have multiple items with same ShaderStageFlagBits value
			vk::PrimitiveTopology topology;
			vk::PolygonMode polygon_mode;
			vk::CullModeFlags cull_mode;
			vk::FrontFace front_face;
		};

		pipeline() = delete;

		explicit pipeline(vk::Device &device, vk::RenderPass &render_pass, const description &desc)
			: vk_device(device)
		{
			create_pipeline(desc, render_pass);
		}

		~pipeline()
		{
			vk_device.destroyPipeline(vk_pipeline);
			vk_device.destroyPipelineLayout(vk_pipeline_layout);
		}

		auto get_pipeline() const -> vk::Pipeline
		{
			return vk_pipeline;
		}

	private:
		void create_pipeline(const description &desc, const vk::RenderPass &render_pass)
		{
			// Assume desc::shaders will always have vertex and fragment shaders

			// Convert shader binary into shader modules
			using shader_stage_module = std::tuple<vk::ShaderStageFlagBits, vk::ShaderModule>;
			auto shader_modules       = std::vector<shader_stage_module>{};
			std::ranges::transform(desc.shaders, std::back_inserter(shader_modules), [&](const description::shader_stage_file &shader) {
				return shader_stage_module{
					std::get<vk::ShaderStageFlagBits>(shader),
					create_shader_module(std::get<description::shader_binary>(shader)),
				};
			});

			// Shader Stages
			// Assume all shaders will have main function as entry point
			auto shader_stage_infos = std::vector<vk::PipelineShaderStageCreateInfo>{};
			std::ranges::transform(shader_modules, std::back_inserter(shader_stage_infos), [](const shader_stage_module &stg_module) {
				return vk::PipelineShaderStageCreateInfo{
					.stage  = std::get<vk::ShaderStageFlagBits>(stg_module),
					.module = std::get<vk::ShaderModule>(stg_module),
					.pName  = "main"
				};
			});
			std::println("Shader Stage Count: {}", shader_stage_infos.size());

			// TODO: Vertex Input Layout equivalent for Vulkan
			auto vertex_input_info = vk::PipelineVertexInputStateCreateInfo{
				.vertexBindingDescriptionCount   = 0,
				.vertexAttributeDescriptionCount = 0,
			};

			// Input Assembly
			auto input_assembly_info = vk::PipelineInputAssemblyStateCreateInfo{
				.topology               = desc.topology,
				.primitiveRestartEnable = false,
			};

			// Viewport
			auto viewport_info = vk::PipelineViewportStateCreateInfo{
				.viewportCount = 1,
				.scissorCount  = 1,
			};

			// Rasterization
			auto rasterization_info = vk::PipelineRasterizationStateCreateInfo{
				.depthClampEnable        = false,
				.rasterizerDiscardEnable = false,
				.polygonMode             = desc.polygon_mode,
				.cullMode                = desc.cull_mode,
				.frontFace               = desc.front_face,
				.depthBiasEnable         = false,
				.lineWidth               = 1.0f,
			};

			// Multisample anti-aliasing
			auto multisample_info = vk::PipelineMultisampleStateCreateInfo{
				.rasterizationSamples = vk::SampleCountFlagBits::e1, // should this be higher for higher msaa?
				.sampleShadingEnable  = false,
			};

			// Color Blend Attachment
			auto color_blend_attach_st = vk::PipelineColorBlendAttachmentState{
				.blendEnable    = false,
				.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA,
			};

			// Color Blend State
			auto color_blend_info = vk::PipelineColorBlendStateCreateInfo{
				.logicOpEnable   = false,
				.logicOp         = vk::LogicOp::eCopy,
				.attachmentCount = 1,
				.pAttachments    = &color_blend_attach_st,
				.blendConstants  = std::array{ 0.f, 0.f, 0.f, 0.f },
			};

			// Dynamic States
			auto dynamic_states = std::vector{
				vk::DynamicState::eViewport,
				vk::DynamicState::eScissor,
			};

			auto dynamic_state_info = vk::PipelineDynamicStateCreateInfo{
				.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
				.pDynamicStates    = dynamic_states.data(),
			};

			// Pipeline Layout
			auto pipeline_layout_info = vk::PipelineLayoutCreateInfo{
				.setLayoutCount         = 0,
				.pushConstantRangeCount = 0,
			};

			vk_pipeline_layout = vk_device.createPipelineLayout(pipeline_layout_info);

			// Finally create Pipeline
			auto gfx_pipeline_info = vk::GraphicsPipelineCreateInfo{
				.stageCount          = static_cast<uint32_t>(shader_stage_infos.size()),
				.pStages             = shader_stage_infos.data(),
				.pVertexInputState   = &vertex_input_info,
				.pInputAssemblyState = &input_assembly_info,
				.pViewportState      = &viewport_info,
				.pRasterizationState = &rasterization_info,
				.pMultisampleState   = &multisample_info,
				.pColorBlendState    = &color_blend_info,
				.pDynamicState       = &dynamic_state_info,
				.layout              = vk_pipeline_layout,
				.renderPass          = render_pass,
				.subpass             = 0,
			};

			auto result_value = vk_device.createGraphicsPipeline(nullptr, gfx_pipeline_info);
			if (result_value.result != vk::Result::eSuccess)
			{
				throw std::runtime_error("Unable to create graphics pipeline");
			}

			vk_pipeline = result_value.value;

			// Destroy the shader modules
			for (auto &&[stg, mod] : shader_modules)
			{
				vk_device.destroyShaderModule(mod);
			}
		}

		auto create_shader_module(const description::shader_binary &shader_bin) -> vk::ShaderModule
		{
			auto ci = vk::ShaderModuleCreateInfo{
				.codeSize = shader_bin.size(),
				.pCode    = shader_bin.data(),
			};

			return vk_device.createShaderModule(ci);
		}

	private:
		vk::Device vk_device;
		vk::PipelineLayout vk_pipeline_layout;
		vk::Pipeline vk_pipeline;
	};
}
