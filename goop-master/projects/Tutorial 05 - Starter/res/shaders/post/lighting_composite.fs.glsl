#version 440

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec2 inScreenCoords;

layout(location = 0) out vec4 outColor;

uniform sampler2D xImage;

// We take in our color GBuffer and our HDR lighting accumulation
layout(binding = 1) uniform sampler2D a_GColor;
layout(binding = 2) uniform sampler2D a_HdrLightAccum;

// This controls our camera's exposure (default 1) Should be lower for bright scenes, and high for dim scenes
uniform float a_Exposure;

// Performs our tone mapping, taking a high dynamic range input and mapping it to a low range (0-1) value
vec3 ToneMap(vec3 color, float exposure) {
	const float gamma = 2.2; // Average gamma of most displays (NOTE: This can be a uniform, to allow for gamma controls)
	vec3 result = vec3(1.0) - exp(-color * exposure);
	result = pow(result, vec3(1.0 / gamma));

	return result;
}

void main() {
	TODO: Read through this file and understand what's going on
	// Simple multiply for base color and lighting
	vec4 color = texture(a_GColor, inUV) * texture(a_HdrLightAccum, inUV);
	// Tonemap value into the 0-1 range, and output it as a solid color
	outColor = vec4(ToneMap(color.rgb, a_Exposure), 1.0);
}