struct VSOUPUT
{
	float4 position : SV_Position;
	float4 color : COLOR0;
};

VSOUPUT main(uint vertex_id : SV_VertexID)
{
	float3 tri_pos[3] = {
		{1.f, 1.f, 0.f},
		{-1.f, 1.f, 0.f},
		{0.f, 0.f, 1.f},
	};

	float4 tri_col[3] = {
		{1.f, 0.f, 0.f, 1.f},
		{0.f, 1.f, 0.f, 1.f},
		{0.f, 0.f, 1.f, 1.f},
	};

	VSOUPUT output = {
		float4(tri_pos[vertex_id], 1.f),
		tri_col[vertex_id]
	};
	

	return output;
}