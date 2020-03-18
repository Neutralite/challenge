#pragma once
#include "TextureEnums.h"
#include "IGraphicsResource.h"
#include <glm/glm.hpp>

namespace florp {
	namespace graphics {

		/*
		 * Represents all the information required to set up a texture sampler
		 */
		struct SamplerDesc {
			/*
			 * The wrap parameter for the horizontal (x) texture axis
			 * @default WrapMode::Repeat
			 */
			WrapMode WrapS = WrapMode::Repeat;
			/*
			 * The wrap parameter for the vertical (y) texture axis
			 * @default WrapMode::Repeat
			 */
			WrapMode WrapT = WrapMode::Repeat;
			/*
			 * The wrap parameter for the depth (z) texture axis
			 * @default WrapMode::Repeat
			 */
			WrapMode WrapR = WrapMode::Repeat;
			/*
			 * The minification filter to use for the texture if no sampler is in use
			 * @default MinFilter::NearestMipLinear
			 */
			MinFilter MinFilter = MinFilter::NearestMipLinear;
			/*
			 * The magnification filter to use for the texture if no sampler is in use
			 * @default MagFilter::Linear
			 */
			MagFilter MagFilter = MagFilter::Linear;
			/*
			 * The default border color for this texture when using WrapMode::ClampToBorder, and no
			 * sampler is specified
			 * @default (0,0,0,1)
			 */
			glm::vec4 BorderColor = glm::vec4(0.0f);
			/*
				The default amount of anisotropic filtering on this texture
				@default 1.0f
			*/
			float     MaxAnisotropy = 1.0f;
		};

		/*
		 * Defines the sampling parameters to use when sampling data from a texture
		 */
		class TextureSampler : public IGraphicsResource {
		public:
			GraphicsClass(TextureSampler);

			/*
			 * Creates a new sampler from the given description
			 * @param desc The parameters to use for this sampler
			 */
			TextureSampler(const SamplerDesc& desc = SamplerDesc());
			~TextureSampler();

			/*
			 * Applies this sampler to be used on the given texture slot, any texture bound
			 * to that slot will be sampled following the parameters set in this sampler
			 * @param slot The slot to apply this sampler to
			 */
			void Bind(uint32_t slot);
			/*
			 * Removes the sampler on the given texture slot, the texture's settings will then be
			 * used for any sampling operations
			 * @param slot The slot to remove the sampler from
			 */
			static void Unbind(uint32_t slot);

			const SamplerDesc& GetDescription() const { return myDesc; }

		private:
			SamplerDesc myDesc;
		};
		
	}
}