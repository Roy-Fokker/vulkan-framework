
[[vk::image_format("rgba16f")]]
RWTexture2D<float4> image : register(u0, space0);

[numthreads(16, 16, 1)]
void main(uint3 thread_id : SV_DispatchThreadID)
{
	int2 texel_coord = (int2)thread_id.xy;
	int2 size;
	image.GetDimensions(size.x, size.y);

	if (texel_coord.x < size.x && texel_coord.y < size.y)
	{
		float4 color = float4(0.0, 0.0, 0.0, 1.0);
		if (thread_id.x != 0 && thread_id.y != 0)
		{
			color.x = float(texel_coord.x) / (size.x);
			color.y = float(texel_coord.y) / (size.y);
		}
		image[texel_coord] = color;
	}
}

// https://docs.vulkan.org/guide/latest/high_level_shader_language_comparison.html#_image_formats
