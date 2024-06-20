module;

#include <cassert>

export module vfw:pipelines;

import std;

export namespace vfw
{
	enum class shader_stage
	{
		vertex,
		pixel,
		compute,
	};

	class pipeline final
	{
	public:
		pipeline()                               = delete;
		pipeline(const pipeline &src)            = delete;
		pipeline &operator=(const pipeline &src) = delete;
		pipeline(pipeline &&src)                 = delete;
		pipeline &operator=(pipeline &&src)      = delete;

		pipeline(vk::Device device, vk::DescriptorSetLayout &set_layout)
			: device(device)
		{
			auto pl_ci = vk::PipelineLayoutCreateInfo{
				.setLayoutCount = 1,
				.pSetLayouts    = &set_layout,
			};

			layout = device.createPipelineLayout(pl_ci);
		}

		~pipeline()
		{
			device.waitIdle();

			device.destroyPipeline(pl);
			device.destroyPipelineLayout(layout);
		}

		void add_shader(shader_stage stage, std::span<uint32_t> data)
		{
			auto make_shader_module = [&](std::span<uint32_t> shader_bin) {
				auto ci = vk::ShaderModuleCreateInfo{
					.codeSize = shader_bin.size(),
					.pCode    = shader_bin.data(),
				};

				return device.createShaderModule(ci);
			};

			auto translate_to_vk_flage = [](shader_stage stage) -> vk::ShaderStageFlagBits {
				using enum vk::ShaderStageFlagBits;
				switch (stage)
				{
				case shader_stage::vertex:
					return eVertex;
				case shader_stage::pixel:
					return eFragment;
				case shader_stage::compute:
					return eCompute;
				}
				return {};
			};

			auto shader_module = make_shader_module(data);
			auto stage_info    = vk::PipelineShaderStageCreateInfo{
				   .stage  = translate_to_vk_flage(stage),
				   .module = shader_module,
				   .pName  = "main",
			};

			if (stage == shader_stage::compute)
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