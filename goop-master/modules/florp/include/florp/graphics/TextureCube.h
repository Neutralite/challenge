#pragma once
#include <EnumToString.h>
#include "TextureEnums.h"
#include "ITexture.h"
#include <GLM/vec2.hpp>

namespace florp {
	namespace graphics {

		/*
		0 	GL_TEXTURE_CUBE_MAP_POSITIVE_X
		1 	GL_TEXTURE_CUBE_MAP_NEGATIVE_X
		2 	GL_TEXTURE_CUBE_MAP_POSITIVE_Y
		3 	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y
		4 	GL_TEXTURE_CUBE_MAP_POSITIVE_Z
		5 	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
		 */
		ENUM(CubeMapFace, GLint,
			PosX = 0,
			NegX = 1,
			PosY = 2,
			NegY = 3,
			PosZ = 4,
			NegZ = 5
		);

		/*
		 * Indicates what method was used to pack a cubemap into a single texture file
		 */
		enum TextureCubePackMode {
			PackMode_None = 0,
			/*
				+z
			 -x -y +x +y
				-z
			*/
			PackMode_HorizCross = 1,
			/*
			 * +x -x +y -y +z -z
			 */
			PackMode_HorizLine = 2,
			/*
			   +x
			   -x
			   +y
			   -y
			   +z
			   -z
			*/
			PackMode_VertLine = 3,
			// The default pack mode is horizontal cross
			PackMode_Default = PackMode_HorizCross
		};
		
		struct TextureCubeDesc {
			uint32_t       Size   = 0;
			InternalFormat Format = InternalFormat::RGBA8;
		};

		/*
		 * Represents a texture cube made up of 6 2D faces. Note that this IS NOT a 3D texture
		 */
		class TextureCube : public ITexture {
		public:
			GraphicsClass(TextureCube);

			/*
			 * Creates a new texture cube with the given parameters
			 * @param desc The parameters for this texture cube
			 */
			TextureCube(const TextureCubeDesc& desc);
			virtual ~TextureCube();

			/*
			 * Loads a single face with data, note that width and height must match the size of the texture cube's faces
			 * @param width The width of the texture data to load
			 * @param height The height of the texture data to load
			 * @param face The face to load the data into
			 * @param format The pixel format of the data being loaded (ex: RGBA), must be compatible with the internal format
			 * @param type The underlying element type of the pixel data being loaded (ex: byte)
			 * @param date A pointer to the underlying pixel data to load
			 */
			void LoadData(uint32_t width, uint32_t height, CubeMapFace face, PixelFormat format, PixelType type, void* data);

			/*
			 * Loads all 6 faces from separate texture files
			 * @param faceFiles An array of paths to load into the texture cube, the indexing of the face matches the values of CubeMapFace
			 * @param flipVertically An array of bools indicating what faces should be flipped vertically
			 */
			static Sptr LoadFromFiles(const std::string faceFiles[6], bool flipVertically[6]);

			/*
			 * Loads a texture cube from a single texture file with the given packing
			 * @param path The path to the image to unpack
			 * @param packMode The method in which the cubemap was packed into the texture
			 * @returns A shared pointer to a texture cube loaded from the given file
			 */
			static Sptr LoadFromFile(const std::string& path, TextureCubePackMode packMode);
			
			/*
			 * Gets the descriptor of this texture cube
			 */
			const TextureCubeDesc& GetDescription() const { return myDesc; }

		protected:
			TextureCubeDesc myDesc;

			/*
			 * Handles taking a 2D texture and packing it into the correct format for OpenGL to read it as a cubemap
			 * @param data The texture data to unpack from
			 * @param texSize The dimensions of the texture data
			 * @param pixelSize The size of a single texel element
			 * @param out faceSize A reference to the value that will receive the size of a single cube face
			 * @param packMode The arrangement of faces in the texture data
			 */
			static void* __Pack(void* data, glm::ivec2 texSize, size_t pixelSize, uint32_t& faceSize, TextureCubePackMode packMode);
		};
		
	}
}
