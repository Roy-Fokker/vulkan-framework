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
			auto translate_to_vk_flage = [](types::shader_stage stage) -> vk::ShaderStageFlagBits {
				using enum vk::ShaderStageFlagBits;
				switch (stage)
				{
					using enum types::shader_stage;
				case vertex:
					return eVertex;
				case pixel:
					return eFragment;
				case compute:
					return eCompute;
				}
				return {};
			};

			auto shader_module = make_shader_module(device, data);
			auto stage_info    = vk::PipelineShaderStageCreateInfo{
				   .stage  = translate_to_vk_flage(stage),
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
}