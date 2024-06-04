module;

#include <vma/vk_mem_alloc.h>

export module vfw:types;

export namespace vfw::types
{
	struct allocated_image
	{
		vk::Image image;
		vk::ImageView view;
		VmaAllocation allocation;
		vk::Extent3D extent;
		vk::Format format;
	};
}