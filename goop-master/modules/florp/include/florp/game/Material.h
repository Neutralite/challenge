#pragma once
#include <string>
#include <memory>
#include "florp/graphics/Shader.h"
#include "florp/graphics/ITexture.h"
#include "florp/graphics/TextureSampler.h"
#include "florp/graphics/RasterizerState.h"

namespace florp {
	namespace game {
		
		/*
		 * Represents settings for a shader, such as uniform and texture settings
		 */
		class Material {
		public:
			typedef std::shared_ptr<Material> Sptr;

			/*
			 * The state for the rasterizer to be in when using this material
			 */
			graphics::RasterizerState RasterState;
			/*
			 * Whether or not this material casts shadows (is rendered by shadow casting lights)
			 */
			bool IsShadowCaster;
			
			Material(const graphics::Shader::Sptr& shader) : IsShadowCaster(true) { myShader = shader; RasterState = graphics::RasterizerState(); }
			virtual ~Material() = default;

			/*
			 * Gets the shader that this material is using
			 */
			const graphics::Shader::Sptr& GetShader() const { return myShader; }
			/*
			 * Applies this material to the current renderer
			 */
			virtual void Apply();

			/*
			 * Creates a duplicate of this material, with all the same values set
			 */
			Sptr Clone();

			/*
			 * Sets a uniform in this material
			 * @param name The name of the uniform to set
			 * @param value The value to set the uniform to
			 */
			void Set(const std::string& name, const glm::mat4& value) { myMat4s[name] = value; }
			/*
			 * Sets a uniform in this material
			 * @param name The name of the uniform to set
			 * @param value The value to set the uniform to
			 */
			void Set(const std::string& name, const glm::vec4& value) { myVec4s[name] = value; }
			/*
			 * Sets a uniform in this material
			 * @param name The name of the uniform to set
			 * @param value The value to set the uniform to
			 */
			void Set(const std::string& name, const glm::vec3& value) { myVec3s[name] = value; }
			/*
			 * Sets a uniform in this material
			 * @param name The name of the uniform to set
			 * @param value The value to set the uniform to
			 */
			void Set(const std::string& name, const glm::vec2& value) { myVec2s[name] = value; }
			/*
			 * Sets a uniform in this material
			 * @param name The name of the uniform to set
			 * @param value The value to set the uniform to
			 */
			void Set(const std::string& name, int value) { myInts[name] = value; }
			/*
			 * Sets a uniform in this material
			 * @param name The name of the uniform to set
			 * @param value The value to set the uniform to
			 */
			void Set(const std::string& name, const float& value) { myFloats[name] = value; }

			/*
			 * Sets a texture uniform in this material
			 * @param name The name of the uniform to set
			 * @param value The value to set the uniform to
			 * @param sampler A pointer to the sampler to use for this texture, or nullptr to use the texture's 
			 */
			void Set(const std::string& name, const graphics::ITexture::Sptr& value, const graphics::TextureSampler::Sptr& sampler = nullptr) {
				myTextures[name] = { value, sampler };
			}

		protected:
			struct TextureInfo {
				graphics::ITexture::Sptr       Texture;
				graphics::TextureSampler::Sptr Sampler;
			};
			graphics::Shader::Sptr myShader;
			std::unordered_map<std::string, glm::mat4> myMat4s;
			std::unordered_map<std::string, glm::vec4> myVec4s;
			std::unordered_map<std::string, glm::vec3> myVec3s;
			std::unordered_map<std::string, glm::vec2> myVec2s;
			std::unordered_map<std::string, int>       myInts;
			std::unordered_map<std::string, float>     myFloats;

			std::unordered_map<std::string, TextureInfo> myTextures;
		};
	}
}
