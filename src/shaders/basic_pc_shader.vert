#version 450

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

layout(push_constant) uniform Push
{
	vec2 offset;
}
push;

void main()
{
	vec2 pos    = { inPosition.x + push.offset.x, inPosition.y + push.offset.y };
	gl_Position = vec4(pos, 0.0, 1.0);
	fragColor   = inColor;
}