module;

#include <vma/vk_mem_alloc.h>

export module vfw:types;

import std;

export namespace vfw::types
{
	struct compute_push_constants
	{
		glm::vec4 data1;
		glm::vec4 data2;
		glm::vec4 data3;
		glm::vec4 data4;
	};

	enum class shader_stage
	{
		vertex,
		pixel,
		compute,
	};

	auto to_vk_flag(shader_stage stage) -> vk::ShaderStageFlagBits
	{
		switch (stage)
		{
			using enum vk::ShaderStageFlagBits;
			using enum types::shader_stage;
		case vertex:
			return eVertex;
		case pixel:
			return eFragment;
		case compute:
			return eCompute;
		}
		return {};
	}

	struct shader_module
	{
		shader_stage stage;
		vk::ShaderModule shdr;
	};

	enum class blending_mode
	{
		none,
		additive,
		alpha_blend,
	};
}