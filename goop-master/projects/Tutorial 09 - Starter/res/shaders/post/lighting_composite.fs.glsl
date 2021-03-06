#version 440

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec2 inScreenCoords;

layout(location = 0) out vec4 outColor;

uniform sampler2D xImage;

layout(binding = 1) uniform sampler2D a_GColor;
layout(binding = 2) uniform sampler2D a_HdrLightAccum;
layout(binding = 3) uniform sampler2D a_GEmissive; // NEW

uniform float a_Exposure;

vec3 ToneMap(vec3 color, float exposure) {
	const float gamma = 2.2;
	vec3 result = vec3(1.0) - exp(-color * exposure);
	result = pow(result, vec3(1.0 / gamma));
	return result;
}

void main() {
	vec4 color = /* TODO: Add the HDR and emissive, and multiply the result by the albedo */; // Updated
	outColor = vec4(ToneMap(color.rgb, a_Exposure), 1.0);
}