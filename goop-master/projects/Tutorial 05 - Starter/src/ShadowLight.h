#pragma once
#include <GLM/glm.hpp>
#include "FrameBuffer.h"

/*
 * Stores the information required to render with a camera. Since this is a component of a gameobject,
 * we do not need a view matrix (we can take the inverse of the matrix that it is attached to).
 *
 * Later on, we can add things like render targets for the camera to render to
 */

static_assert(false); // You should look at this structure and understand what's going on here!
struct ShadowLight {
	// The render target that this camera will render to
	FrameBuffer::Sptr                  ShadowBuffer;
	// The mask to use for ignoring sections of this lights view from evaluation (for instance, to easily make a light look like it's behind a grate)
	florp::graphics::Texture2D::Sptr   Mask;
	// An image to be projected onto areas that this light illuminates
	florp::graphics::Texture2D::Sptr   ProjectorImage;

	// The projection matrix for this shadow caster
	glm::mat4         Projection;

	// The color of this light
	glm::vec3 Color;
	// The inverse of the distance that this light propegates
	float     Attenuation;
};