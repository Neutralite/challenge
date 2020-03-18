#include "florp/utils/TextureUtils.h"
#include "Logging.h"

namespace florp {
	namespace utils {

		void TextureUtils::CopyRegion(void* dst, glm::ivec4 dstBounds, glm::ivec2 dstTexSize, void* src, glm::ivec4 srcBounds, glm::ivec2 srcTexSize, size_t pixelSize) {
			LOG_ASSERT(dstBounds.z == srcBounds.z, "Destination and source width are not identical");
			LOG_ASSERT(dstBounds.w == srcBounds.w, "Destination and source width are not identical");

			for (int y = 0; y < dstBounds.w; y++) {
				size_t srcOffset = pixelSize * ((srcBounds.y + y) * srcTexSize.x + srcBounds.x); // The offset into the source of the pixel
				size_t dstOffset = pixelSize * ((dstBounds.y + y) * dstTexSize.x + dstBounds.x); // The offset into the destination 
				memcpy((char*)dst + dstOffset, (char*)src + srcOffset, pixelSize * dstBounds.z);
			}
		}
		
	}
}
