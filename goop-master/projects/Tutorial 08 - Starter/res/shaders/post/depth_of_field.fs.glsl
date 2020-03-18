#version 440

// Modified from:
// http://tuxedolabs.blogspot.com/2018/05/bokeh-depth-of-field-in-single-pass.html

layout(location = 0) in vec2 inUV;
layout(location = 1) in vec2 inScreenCoords;

layout(location = 0) out vec4 outColor;

// Our color buffer to sample from
layout(binding = 0) uniform sampler2D a_Sampler;pl
// The depth buffer to use (non-linearized)
layout(binding = 1) uniform sampler2D a_Depth;

// The current focal depth (linear)
uniform float a_FocalDepth;
// The distance in world units between the camera's lense and it's sensor
uniform float a_LenseDistance;
// The aperture of the camera (default is 20) This can be thought of as the inverse of your camera's F-Stop
uniform float a_Aperture;
// The inverse of the camera's projection matrix
uniform mat4 a_ProjectionInv;

const float GOLDEN_ANGLE = 2.39996323;
const float MAX_BLUR_RADIUS = 20;
const float RAD_SCALE = 0.5;

// Converts a screen space coord and a raw depth value into a world-space distance
// @param screen The screen-space coordinate to convert
// @param rawValue The raw, non-linear depth value to convert
// @returns A distance to the camera in world units
float DepthToDist(vec2 screen, float rawValue) {
	vec4 screenPos = vec4(screen.x, screen.y, rawValue, 1.0) * 2.0 - 1.0;
	vec4 viewPosition = a_ProjectionInv * screenPos;
	return -(viewPosition.z / viewPosition.w);
}

/*
* Calculates the Circle of Confusion for a given depth value
* @param depth The depth of the fragment to caluculate for (in world units)
* @param focalPlane The distance from the lense to the focal plane (in world units)
* @param focalLength The focal length parameter (calculated as 1/F = 1/focalPlane + 1/distToSensor)
* @see http://fileadmin.cs.lth.se/cs/Education/EDAN35/lectures/12DOF.pdf
*/
float getBlurSize(float depth, float focalPlane, float focalLength) {
	// TODO: calculate our Circle of Confusion
}

/*
* Calculates our color for the depth of field effect
* @param texCoord The UV coordinate to solve for
* @param focusPoint The distance from the lense to the focal plane (in world units)
* @param focalLength The focal length parameter (calculated as 1/F = 1/focalPlane + 1/distToSensor)
*/
vec3 depthOfField(vec2 texCoord, float focusPoint, float focusLength) {
    // Determines the size of single texel
    vec2 texelSize = 1.0 / textureSize(a_Depth, 0);

    // Get our depth into view space, and use that to calculate our circle of confusion
	float centerDepth = DepthToDist(texCoord, texture(a_Depth, texCoord).r);
    float centerCOC = getBlurSize(centerDepth, focusPoint, focusLength);

    // Initialize out color and total number of samples
    vec3 color = texture(a_Sampler, texCoord).rgb;
    float tot = 1.0;

    // We'll blur our fragment outward in a circle
    float radius = RAD_SCALE;
	// We'll rotate around out point by a constant parameter (the golden angle)
	// The golden angle is a constant that will help our samples not fall on pixel boundries, and reduce
	// artifacts from our blurring methodology
    for (float ang = 0.0; radius < min(a_Aperture, MAX_BLUR_RADIUS); ang += GOLDEN_ANGLE)
    {
		// TODO: Blur in samples around this pixel
    }
    // We'll return the average of all our colors
    return color /= tot;
}

void main() {
    // Calculate our focal length
	float focalLength = ???;
    // Perform our DOF blurring
    vec3 dof = depthOfField(inUV, a_FocalDepth, focalLength);
    // Return the result
    outColor = vec4(dof, 1.0);
}