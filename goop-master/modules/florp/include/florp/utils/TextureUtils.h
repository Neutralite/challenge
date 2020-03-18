#pragma once
#include <GLM/glm.hpp>

namespace florp {
	namespace utils {

		class TextureUtils {
		public:
			/*
			 * Copies a segment of a texture from one region to another. Note that src and dst are expected to be different buffers
			 * @param dst The destination buffer
			 * @param dstBounds The bounds of the destination, given as (x, y, width, height)
			 * @param dstTexSize The size of the destination texture
			 * @param src The source buffer for the copy
			 * @param srcBounds The bounds of the source data, given as (x, y, width, height)
			 * @param srcTexSize The size of the source texture
			 * @param pixelSize The stride of a single texel element
			 */
			static void CopyRegion(void* dst, glm::ivec4 dstBounds, glm::ivec2 dstTexSize, void* src, glm::ivec4 srcBounds, glm::ivec2 srcTexSize, size_t pixelSize);
		};
		
	}
}
