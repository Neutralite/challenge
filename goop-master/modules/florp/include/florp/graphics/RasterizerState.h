#pragma once
#include <EnumToString.h>
#include "glad/glad.h"

namespace florp
{
	namespace graphics
	{
		ENUM(FillMode, uint32_t,
			Point = GL_POINT,
			Line  = GL_LINE,
			Fill  = GL_FILL
			);

		ENUM(CullMode, uint32_t,
			None  = GL_NONE,
			Front = GL_FRONT,
			Back  = GL_BACK,
			Both  = GL_FRONT_AND_BACK
			);

		ENUM(BlendFunc, uint32_t,
			Zero = GL_ZERO,
			One  = GL_ONE,
			SrcCol = GL_SRC_COLOR,
			OneMinusSrcCol = GL_ONE_MINUS_SRC_COLOR,
			DstCol = GL_DST_COLOR,
			OneMinusDstCol = GL_ONE_MINUS_DST_COLOR,
			SrcAlpha = GL_SRC_ALPHA,
			OneMinusSrcAlpha = GL_ONE_MINUS_SRC_ALPHA,
			DstAlpha = GL_DST_ALPHA,
			OneMinusDstAlpha = GL_ONE_MINUS_DST_ALPHA,
			SrcAlphaSaturate = GL_SRC_ALPHA_SATURATE
			);

		ENUM(BlendEquation, uint32_t,
			Add = GL_FUNC_ADD,
			Sub = GL_FUNC_SUBTRACT,
			ReverseSub = GL_FUNC_REVERSE_SUBTRACT,
			Min = GL_MIN,
			Max = GL_MAX
			);

		struct BlendState {
			bool          BlendEnabled   = false; // FIXED
			BlendEquation RgbBlendFunc   = BlendEquation::Add;
			BlendEquation AlphaBlendFunc = BlendEquation::Add;
			BlendFunc     SrcRgb         = BlendFunc::One;
			BlendFunc     DstRgb         = BlendFunc::One;
			BlendFunc     SrcAlpha       = BlendFunc::Zero;
			BlendFunc     DstAlpha       = BlendFunc::Zero;
		};
		
		const BlendState AlphaBlendState = {
			true,
			BlendEquation::Add,
			BlendEquation::Add,
			BlendFunc::One,
			BlendFunc::OneMinusSrcAlpha,
			BlendFunc::One,
			BlendFunc::One
		};

		/*
		 * Represents the state of the graphics rasterize, such as the culling, fill modes, blending, etc...
		 */
		struct RasterizerState
		{
			FillMode FrontFaceFill = FillMode::Fill;
			FillMode BackFaceFill  = FillMode::Fill;
			CullMode CullMode      = CullMode::Back;
			BlendState Blending    = BlendState();
		};
		
	}
}
