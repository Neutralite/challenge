#include "florp/graphics/Texture2D.h"
#include "Logging.h"
#include <GLM/gtc/type_ptr.hpp>
#include <filesystem>
#include "stb_image.h"

namespace florp { namespace graphics {
	uint32_t Texture2D::MaxTextureSize = 0;
	uint32_t Texture2D::MaxNumSamples  = 1;

	Texture2D::Texture2D(const Texture2dDescription& description) {
		if (MaxTextureSize == 0) {
			int size = 0;
			glGetIntegerv(GL_MAX_TEXTURE_SIZE, &size);
			MaxTextureSize = size;
			LOG_INFO("Maximum texture size on this renderer is {} texels", MaxTextureSize);

			glGetIntegerv(GL_MAX_SAMPLES, &size);
			MaxNumSamples = size;
			LOG_INFO("Maximum number of samples is {}", MaxNumSamples);
		}
					
		// Ensure that the texture size is valid
		LOG_ASSERT(description.Width > 0, "Texture width must be > 0");
		LOG_ASSERT(description.Height > 0, "Texture height must be > 0");
		LOG_ASSERT(description.Width <= MaxTextureSize, "Texture width must be < {}", MaxTextureSize);
		LOG_ASSERT(description.Height <= MaxTextureSize, "Texture height must be < {}", MaxTextureSize);

		// Store the texture description
		myDescription = description;
		// Creates the texture on the GPU
		__RecreateTexture();
	}

	Texture2D::~Texture2D() {
		LOG_INFO("Deleting texture with ID: {}", myRendererID);
		glDeleteTextures(1, &myRendererID);
	}

	Texture2D::Sptr Texture2D::LoadFromFile(const std::string& filePath, bool loadAlpha, bool generateMipmaps, bool linearinze)
	{
		// Load the image data from the file
		int width, height, numChannels;
		int targetChannels =
			linearinze ? 3 :
			(loadAlpha ? 4 : 0);
				
		void* data = stbi_load(filePath.c_str(), &width, &height, &numChannels, targetChannels);
		if (targetChannels != 0)
			numChannels = targetChannels;
		
		// We have data!
		InternalFormat internal_format;
		PixelFormat    image_format;
		switch (numChannels) {
			case 1:
				internal_format = InternalFormat::R8;
				image_format = PixelFormat::Red;
				break;
			case 2:
				internal_format = InternalFormat::RG8;
				image_format = PixelFormat::Rg;
				break;
			case 3:
				internal_format = InternalFormat::RGB8;
				image_format = PixelFormat::Rgb;
				break;

			case 4:
				internal_format = InternalFormat::RGBA8;
				image_format = PixelFormat::Rgba;
				break;

			default:
				LOG_WARN("Unsupported texture format. Texture may look strange. ({})", filePath);
				internal_format = InternalFormat::RGBA8;
				image_format = PixelFormat::Rgba;
				break;
		}

		LOG_ASSERT((numChannels * width) % 4 == 0, "The alignment of a horizontal line in a texture must be 4 bytes! Check that your number of channels * your width is a multiple of 4");
		
		// Only create the texture if we actually got data out
		if (data != nullptr && width != 0 && height != 0 && numChannels != 0) {
			// Create our texture description
			Texture2dDescription desc = Texture2dDescription();
			desc.Width = width;
			desc.Height = height;
			desc.Format = internal_format;
			desc.MipmapLevels = generateMipmaps ? 0 : 1;

			// Define what data will be loaded into the texture, and it's format
			Texture2dData texData = Texture2dData();
			texData.Width = width;
			texData.Height = height;
			texData.Format = image_format;
			texData.Type = PixelType::UByte;
			texData.Data = data;

			// Create the image, and load the data into it
			Sptr result = std::make_shared<Texture2D>(desc);
			result->SetData(texData);
			result->SetDebugName(std::filesystem::path(filePath).filename().string());

			// Free the underlying image data and return the image
			stbi_image_free(texData.Data);
			return result;
		}
		else {
			// Free the data just, to be safe
			stbi_image_free(data);
			LOG_WARN("Failed to load image from \"{}\"", filePath);
			return nullptr;
		}
	}
	
	void Texture2D::SetData(const Texture2dData& data) {
		// We cannot upload data to a multi-sampled texture, it must be rendered to
		LOG_ASSERT(myDescription.NumSamples == 1, "Cannot upload data to a multi-sampled texture!"); // NEW
		
		// Make sure that the dimensions of the data match the dimension of the texture
		LOG_ASSERT(data.Width == myDescription.Width, "Data width does not match texture width");
		LOG_ASSERT(data.Height == myDescription.Height, "Data width does not match texture width");


		// Load the data into the texture
		glTextureSubImage2D(myRendererID, 0, 0, 0, myDescription.Width, myDescription.Height, (GLenum)data.Format, (GLenum)data.Type, data.Data);
		
		// Generate the texture's mip maps
		if (myDescription.MipmapLevels > 1)
			glGenerateTextureMipmap(myRendererID);
	}

	void* Texture2D::GetData(PixelFormat format, PixelType type) const {
		size_t dataSize = GetTexelSize(format, type) * myDescription.Width * myDescription.Height;
		void* result = malloc(dataSize);
		glGetTextureImage(myRendererID, 0, (GLenum)format, (GLenum)type, dataSize, result);
		return result;
	}

	void Texture2D::__RecreateTexture() {		
		// If our miplevels is 0, we need to calculate how many mipmap levels we should generate
		myDescription.MipmapLevels = myDescription.MipmapLevels == 0 ?
			(1 + floor(log2(glm::max(myDescription.Width, myDescription.Height)))) :
			(myDescription.MipmapLevels);

		// Clamp our number of samples to the valid range
		myDescription.NumSamples = glm::clamp(myDescription.NumSamples, 1u, MaxNumSamples); 
		myDescription.MipmapLevels = myDescription.NumSamples > 1 ? 1 : myDescription.MipmapLevels;
				
		glCreateTextures(myDescription.NumSamples > 1 ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, 1, &myRendererID);

		if (myDescription.NumSamples == 1) {
			glTextureStorage2D(myRendererID, myDescription.MipmapLevels, (GLenum)myDescription.Format, myDescription.Width, myDescription.Height);
			
			glTextureParameteri(myRendererID, GL_TEXTURE_WRAP_S, (GLenum)myDescription.WrapS);
			glTextureParameteri(myRendererID, GL_TEXTURE_WRAP_T, (GLenum)myDescription.WrapT);
			glTextureParameteri(myRendererID, GL_TEXTURE_MIN_FILTER, (GLenum)myDescription.MinFilter);
			glTextureParameteri(myRendererID, GL_TEXTURE_MAG_FILTER, (GLenum)myDescription.MagFilter);

			glTextureParameterfv(myRendererID, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(myDescription.BorderColor));

			if (myDescription.Aniso != 1.0f)
				glTextureParameterf(myRendererID, GL_TEXTURE_MAX_ANISOTROPY, myDescription.Aniso);
		}
		else
			glTextureStorage2DMultisample(myRendererID, myDescription.NumSamples, (GLenum)myDescription.Format, myDescription.Width, myDescription.Height, true);	

	}
 } }