#pragma once
#include "ITexture.h"
#include <GLM/detail/type_vec4.hpp>

namespace florp {
	namespace graphics {
		
		/*
		 * Represents the data that will be transferred from the CPU to the GPU when uploading 2D texture data
		 */
		struct Texture2dData {
			uint32_t    Width, Height;
			PixelFormat Format;
			PixelType   Type;
			void* Data;
		};

		/*
			Holds information representing the internal state of a 2D texture
		*/
		struct Texture2dDescription {
			/*
			 * The width or height of the texture, in texels
			 * @default 0
			 */
			uint32_t       Width, Height;
			/*
			 * The number of samples to use if we are using multisampling for this texture
			 * Default is 1
			 */
			uint32_t       NumSamples; // NEW
			/*
				The number of mip levels to generate for this texture, 0 will calculate the number of miplevels,
				1 will disable mipmapping
				@default 0
			*/
			uint32_t       MipmapLevels;
			/*
			 * The sized internal format of the texture
			 * @default InternalFormat::RGBA8
			 */
			InternalFormat Format;
			/*
			 * The wrap parameter for the horizontal texture axis
			 * @default WrapMode::Repeat
			 */
			WrapMode       WrapS;
			/*
			 * The wrap parameter for the vertical texture axis
			 * @default WrapMode::Repeat
			 */
			WrapMode       WrapT;
			/*
			 * The minification filter to use for the texture if no sampler is in use
			 * @default MinFilter::NearestMipLinear
			 */
			MinFilter      MinFilter;
			/*
			 * The magnification filter to use for the texture if no sampler is in use
			 * @default MagFilter::Linear
			 */
			MagFilter      MagFilter;
			/*
				The default amount of anisotropic filtering on this texture
				@default 1.0f
			*/
			float          Aniso;
			/*
			 * The default border color for this texture when using WrapMode::ClampToBorder, and no
			 * sampler is specified
			 * @default (0,0,0,1)
			 */
			glm::vec4      BorderColor;

			/*
			 * Initializes a texture description to it's default parameters
			 */
			Texture2dDescription() :
				Width(0), Height(0),
				NumSamples(1), // NEW
				MipmapLevels(0),
				Format(InternalFormat::RGBA8),
				WrapS(WrapMode::Repeat), WrapT(WrapMode::Repeat),
				MinFilter(MinFilter::NearestMipLinear),
				MagFilter(MagFilter::Linear),
				Aniso(1.0f),
				BorderColor({ 0.0f, 0.0f, 0.0f, 1.0f }) { }
		};

		/*
		 * Represents a 2D texture
		 * In OpenGL, this is a wrapper around the glTexture2d* functions
		 */
		class Texture2D : public ITexture {
		public:
			GraphicsClass(Texture2D);

			Texture2D(const Texture2dDescription& description);
			virtual ~Texture2D();

			/*
			 * Loads a 2D texture from the given file path
			 * @param filePath The path to a texture asset on disk
			 * @returns A shared pointer for a 2D texture as loaded from the given path
			 */
			static Texture2D::Sptr LoadFromFile(const std::string& filePath, bool loadAlpha = true, bool generateMipMaps = true, bool linearize = false);
			
			// Mutators
		public:
			/*
			 * Sets the data in this texture, overwriting any existing data
			 * @param data The new data for this texture, must match the current texture's dimensions
			 */
			void SetData(const Texture2dData& data);

			// Accessors
		public:
			uint32_t GetWidth() const { return myDescription.Width; }
			uint32_t GetHeight() const { return myDescription.Height; }

			bool IsMipmapEnabled() const { return myDescription.MipmapLevels; }
			uint32_t GetMipLevels() const { return myDescription.MipmapLevels; }

			WrapMode GetWrapS() const { return myDescription.WrapS; }
			WrapMode GetWrapT() const { return myDescription.WrapT; }

			MinFilter GetMinFilter() const { return myDescription.MinFilter; };
			MagFilter GetMagFilter() const { return myDescription.MagFilter; };

			float GetAniso() const { return myDescription.Aniso; }

			/*
				Extracts data from this texture with the given format. Note that memory is allocated with malloc,
				and is owned by the calling function. This means that you MUST release the memory with a call to
				free() after you are done with the data.
				@param format The texel format to extract data as
				@param type   The data type of the components of the texel
				@returns A block of data, allocated with malloc, that contains the data in the texture
			*/
			void* GetData(PixelFormat format, PixelType type) const;

		protected:
			Texture2dDescription myDescription;

			// Handles actually allocating space for the texture on the GPU
			void __RecreateTexture();

		private:
			static uint32_t MaxTextureSize;
			static uint32_t MaxNumSamples; // NEW
		};
	}
}