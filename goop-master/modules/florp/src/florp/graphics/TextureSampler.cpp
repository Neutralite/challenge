#include "florp/graphics/TextureSampler.h"
#include <GLM/gtc/type_ptr.hpp>
#include "Logging.h"

namespace florp { namespace graphics
{
	TextureSampler::TextureSampler(const SamplerDesc& desc) {
		myDesc = desc;

		glCreateSamplers(1, &myRendererID);

		glSamplerParameteri(myRendererID, GL_TEXTURE_WRAP_S, (GLenum)myDesc.WrapS);
		glSamplerParameteri(myRendererID, GL_TEXTURE_WRAP_T, (GLenum)myDesc.WrapT);
		glSamplerParameteri(myRendererID, GL_TEXTURE_WRAP_R, (GLenum)myDesc.WrapR);

		glSamplerParameteri(myRendererID, GL_TEXTURE_MIN_FILTER, (GLenum)myDesc.MinFilter);
		glSamplerParameteri(myRendererID, GL_TEXTURE_MAG_FILTER, (GLenum)myDesc.MagFilter);

		glSamplerParameterfv(myRendererID, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(myDesc.BorderColor));

		if (myDesc.MaxAnisotropy != 1.0f)
			glSamplerParameterf(myRendererID, GL_TEXTURE_MAX_ANISOTROPY, myDesc.MaxAnisotropy);
	}

	TextureSampler::~TextureSampler() {
		LOG_INFO("Deleting texture sampler with ID: {}", myRendererID);
		glDeleteSamplers(1, &myRendererID);
	}

	void TextureSampler::Bind(uint32_t slot) {
		glBindSampler(slot, myRendererID);
	}

	void TextureSampler::Unbind(uint32_t slot) {
		glBindSampler(slot, 0);
	}
	
} }
