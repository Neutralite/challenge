#include "florp/graphics/TextureCube.h"
#include "florp/utils/TextureUtils.h"
#include "Logging.h"
#include "stb_image.h"
#include <filesystem>

namespace florp { namespace graphics {

	#pragma region Packing
	
	void Unpack_HorizLine(void* result, void* data, uint32_t faceSize, glm::ivec2 texSize, uint32_t pixelSize) {
		LOG_ASSERT(texSize.x % 6 == 0, "Data width is not a multiple of 6");
		LOG_ASSERT(texSize.x / 6 == faceSize, "Data does not contain 6 faces worth of data");
		LOG_ASSERT(texSize.y == faceSize, "Data height does not match face size");
		glm::ivec2 size = { faceSize, faceSize };

		for (int tex = 0; tex < 6; tex++) {
			glm::ivec4 dstBounds = { 0, faceSize * tex, faceSize, faceSize };
			glm::ivec4 srcBounds = { faceSize * tex, 0, faceSize, faceSize };
			utils::TextureUtils::CopyRegion(result, dstBounds, size, data, srcBounds, texSize, pixelSize);
		}
	}

	void Unpack_VertLine(void* result, void* data, uint32_t faceSize, glm::ivec2 texSize, uint32_t pixelSize) {
		LOG_ASSERT(texSize.y % 6 == 0, "Data height is not a multiple of 6");
		LOG_ASSERT(texSize.y / 6 == faceSize, "Data does not contain 6 faces worth of data");
		LOG_ASSERT(texSize.x == faceSize, "Data width does not match face size");

		glm::ivec2 size = { faceSize, faceSize };

		// Note: we could just copy over the entire buffer, but we will be more explicit here
		for (int tex = 0; tex < 6; tex++) {
			glm::ivec4 dstBounds = { 0, faceSize * tex, faceSize, faceSize };
			glm::ivec4 srcBounds = { 0, faceSize * tex, faceSize, faceSize };
			utils::TextureUtils::CopyRegion(result, dstBounds, size, data, srcBounds, texSize, pixelSize);
		}
	}

	void Unpack_HorizCross(void* result, void* data, uint32_t faceSize, glm::ivec2 texSize, uint32_t pixelSize) {
		LOG_ASSERT(texSize.x % 4 == 0, "Data width is not a multiple of 4");
		LOG_ASSERT(texSize.x / 4 == faceSize, "Data width does not contain 4 faces worth of data");
		LOG_ASSERT(texSize.y % 3 == 0, "Data height is not a multiple of 3");
		LOG_ASSERT(texSize.y / 3 == faceSize, "Data height does not contain 3 faces worth of data");
		
		glm::ivec2 size = { faceSize, faceSize * 6 };
		glm::ivec4 dstBounds; glm::ivec4 srcBounds;

		// pos x
		dstBounds = { 0, faceSize * 0, faceSize, faceSize };
		srcBounds = { faceSize * 2, faceSize, faceSize, faceSize };
		utils::TextureUtils::CopyRegion(result, dstBounds, size, data, srcBounds, texSize, pixelSize);

		// neg x
		dstBounds = { 0, faceSize * 1, faceSize, faceSize };
		srcBounds = { faceSize * 0, faceSize, faceSize, faceSize };
		utils::TextureUtils::CopyRegion(result, dstBounds, size, data, srcBounds, texSize, pixelSize);

		// pos z
		dstBounds = { 0, faceSize * 2, faceSize, faceSize };
		srcBounds = { faceSize * 1, 0, faceSize, faceSize };
		utils::TextureUtils::CopyRegion(result, dstBounds, size, data, srcBounds, texSize, pixelSize);

		// neg z
		dstBounds = { 0, faceSize * 3, faceSize, faceSize };
		srcBounds = { faceSize * 1, faceSize * 2, faceSize, faceSize };
		utils::TextureUtils::CopyRegion(result, dstBounds, size, data, srcBounds, texSize, pixelSize);

		// neg y
		dstBounds = { 0, faceSize * 4, faceSize, faceSize };
		srcBounds = { faceSize * 1, faceSize, faceSize, faceSize };
		utils::TextureUtils::CopyRegion(result, dstBounds, size, data, srcBounds, texSize, pixelSize);

		// pos y
		dstBounds = { 0, faceSize * 5, faceSize, faceSize };
		srcBounds = { faceSize * 3, faceSize, faceSize, faceSize };
		utils::TextureUtils::CopyRegion(result, dstBounds, size, data, srcBounds, texSize, pixelSize);
	}
	
	size_t GetPreferredSize(uint32_t w, uint32_t h, TextureCubePackMode mode) {
		switch (mode) {
			case PackMode_HorizCross:
				return w / 4;
			case PackMode_VertLine:
				return w;
			case PackMode_HorizLine:
				return w / 6;
			default:
				assert(false, "Cannot pick size");
		}
	}

	#pragma endregion
	
	TextureCube::TextureCube(const TextureCubeDesc& desc) {
		myDesc = desc;
		
		GLenum format = (GLenum)myDesc.Format;

		glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &myRendererID);

		glTextureParameteri(myRendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(myRendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(myRendererID, GL_TEXTURE_MIN_FILTER, (GLenum)MinFilter::Linear);
		glTextureParameteri(myRendererID, GL_TEXTURE_MAG_FILTER, (GLenum)MagFilter::Linear);


		glTextureStorage2D(myRendererID, 1, format, myDesc.Size, myDesc.Size);
	}

	TextureCube::~TextureCube() {
		LOG_INFO("Deleting texture with ID: {}", myRendererID);
		glDeleteTextures(1, &myRendererID);
	}

	void TextureCube::LoadData(uint32_t width, uint32_t height, CubeMapFace face, PixelFormat format, PixelType type, void* data) {
		glTextureSubImage3D(myRendererID, 0, 0, 0, (int)face, myDesc.Size, myDesc.Size, 1, (GLenum)format, (GLenum)type, data);
	}

	TextureCube::Sptr TextureCube::LoadFromFiles(const std::string faceFiles[6], bool flipVertically[6]) {
		TextureCubeDesc desc = TextureCubeDesc();
		desc.Format = InternalFormat::RGB8;

		Sptr result = nullptr;

		for (int ix = 0; ix < 6; ix++) {
			stbi_set_flip_vertically_on_load(flipVertically[ix]);

			int width, height, numChannels;
			void* data = stbi_load(faceFiles[ix].c_str(), &width, &height, &numChannels, 3);

			if (desc.Size != 0 && ((width != desc.Size) | (height != desc.Size))) {
				stbi_image_free(data);
				LOG_ASSERT(false, "Image file dimensions do not match the size of this cubemap! ({})", faceFiles[ix]);
			}
			if (width != height) {
				stbi_image_free(data);
				LOG_ASSERT(false, "Image for cubemap must be square! ({})", faceFiles[ix]);
			}

			if (ix == 0) {
				desc.Size = width;
				result = std::make_shared<TextureCube>(desc);
			}

			if (data != nullptr && width != 0 && height != 0 && numChannels != 0) {
				result->LoadData(width, height, (CubeMapFace)ix,
					numChannels == 4 ? PixelFormat::Rgba : PixelFormat::Rgb,
					PixelType::UByte, data);
				stbi_image_free(data);
			}
			else {
				LOG_WARN("Failed to load image from \"{}\"", faceFiles[ix]);
			}
		}

		return result;
	}

	TextureCube::Sptr TextureCube::LoadFromFile(const std::string& path, TextureCubePackMode packMode) 	{
		// Load the image data from the file
		int width, height, numChannels;
		void* data = stbi_load(path.c_str(), &width, &height, &numChannels, 0);
		
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
			LOG_WARN("Unsupported texture format. Texture may look strange. ({})", path);
			internal_format = InternalFormat::RGBA8;
			image_format = PixelFormat::Rgba;
			break;
		}

		// Only create the texture if we actually got data out
		if (data != nullptr && width != 0 && height != 0 && numChannels != 0 && width == height) {
			// Create our texture description
			TextureCubeDesc desc = TextureCubeDesc();
			desc.Format = internal_format;

			// Unpack our texture data into the format that we will need for OpenGL
			void* packedData = __Pack(data, { width, height }, GetTexelSize(image_format, PixelType::UByte), desc.Size, packMode);

			// Create the cube, and load the data into it
			Sptr result = std::make_shared<TextureCube>(desc);
			glTextureSubImage3D(result->myRendererID, 0, 0, 0, 0, desc.Size, desc.Size, 6, *image_format, *PixelType::UByte, packedData);
			
			// Our resource name will be the path that it was loaded from
			result->SetDebugName(std::filesystem::path(path).filename().string());

			// Free the underlying image data and return the image
			stbi_image_free(data);
			free(packedData);
			
			return result;
		}
		else {
			// Free the data just, to be safe
			stbi_image_free(data);
			LOG_WARN("Failed to load image from \"{}\"", path);
			return nullptr;
		}
	}
	
	void* TextureCube::__Pack(void* data, glm::ivec2 texSize, size_t pixelSize, uint32_t& faceSize, TextureCubePackMode packMode /*= PackMode_Default*/)
	{
		faceSize = GetPreferredSize(texSize.x, texSize.y, packMode);
		void* result = malloc(faceSize * faceSize * 6 * pixelSize);
		LOG_TRACE("Packing cubemap texture from a {}x{} texture to a face size of {}", texSize.x, texSize.y, faceSize);
		switch (packMode) {
			case PackMode_HorizLine:   Unpack_HorizLine(result, data, faceSize, texSize, pixelSize);     break;
			case PackMode_VertLine:    Unpack_VertLine(result, data, faceSize, texSize, pixelSize);      break;
			case PackMode_HorizCross:  Unpack_HorizCross(result, data, faceSize, texSize, pixelSize);    break;
		}
		return result;
	}
	
} }
