module;

export module vfw:vertex;

export namespace vfw
{
	struct vertex
	{
		glm::vec2 pos;
		glm::vec3 colr;

		constexpr static auto get_binding_descriptions()
		{
			return std::array{
				vk::VertexInputBindingDescription{
					.binding   = 0,
					.stride    = sizeof(vertex),
					.inputRate = vk::VertexInputRate::eVertex,
				},
			};
		}

		constexpr static auto get_attribute_descriptions()
		{
			return std::array{
				vk::VertexInputAttributeDescription{
					.location = 0,
					.binding  = 0,
					.format   = vk::Format::eR32G32Sfloat,
					.offset   = offsetof(vertex, pos),
				},
				vk::VertexInputAttributeDescription{
					.location = 1,
					.binding  = 0,
					.format   = vk::Format::eR32G32B32Sfloat,
					.offset   = offsetof(vertex, colr),
				},
			};
		}
	};
}