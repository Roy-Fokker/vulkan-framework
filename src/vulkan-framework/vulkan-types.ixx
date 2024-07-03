module;

#include <vma/vk_mem_alloc.h>

export module vfw:types;

export namespace vfw::types
{
	struct compute_push_constants
	{
		glm::vec4 data1;
		glm::vec4 data2;
		glm::vec4 data3;
		glm::vec4 data4;
	};

}