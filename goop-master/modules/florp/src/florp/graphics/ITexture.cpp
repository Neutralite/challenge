#include "florp/graphics/ITexture.h"

namespace florp {
	namespace graphics {
		void ITexture::Bind(uint32_t slot) {
			glBindTextureUnit(slot, myRendererID);
		}	 
	}
}