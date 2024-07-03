// descriptor bindings for the pipeline
[[vk::image_format("rgba16f")]]
RWTexture2D<float4> image : register(u0, space0);

// define the data structure for push constants
struct PushConstants
{
	float4 data1;
	float4 data2;
	float4 data3;
	float4 data4;
};

// make object to hold push constants
[[vk::push_constant]] PushConstants constants;

[numthreads(16, 16, 1)] // size of a workgroup for compute
void main(uint3 thread_id : SV_DispatchThreadID,
          uint3 local_id : SV_GroupThreadID)
{
	int2 texel_coord = (int2)thread_id.xy;
	float4 top_color = constants.data1;
	float4 bottom_color = constants.data2;
	int2 size;
	image.GetDimensions(size.x, size.y);

	if (texel_coord.x < size.x && texel_coord.y < size.y)
	{
		float blend = float(texel_coord.y) / size.y;
		float4 color = lerp(top_color, bottom_color, blend);
		
		image[texel_coord] = color;
	}
}

// https://docs.vulkan.org/guide/latest/high_level_shader_language_comparison.html
// https://anteru.net/blog/2016/mapping-between-HLSL-and-GLSL/
// 