#include "florp/game/Material.h"

namespace florp {
	namespace game {
		
		void Material::Apply() {
			for (auto& kvp : myMat4s)
				myShader->SetUniform(kvp.first, kvp.second);
			for (auto& kvp : myVec4s)
				myShader->SetUniform(kvp.first, kvp.second);
			for (auto& kvp : myVec3s)
				myShader->SetUniform(kvp.first, kvp.second);
			for (auto& kvp : myVec2s)
				myShader->SetUniform(kvp.first, kvp.second);
			for (auto& kvp : myFloats)
				myShader->SetUniform(kvp.first, kvp.second);
			for (auto& kvp : myInts)
				myShader->SetUniform(kvp.first, kvp.second);

			if (RasterState.CullMode != graphics::CullMode::None) {
				glEnable(GL_CULL_FACE);
				glCullFace(*RasterState.CullMode);
			} else {
				glDisable(GL_CULL_FACE);
			}
			
			glPolygonMode(GL_FRONT, *RasterState.FrontFaceFill);
			glPolygonMode(GL_BACK, *RasterState.BackFaceFill);

			if (RasterState.Blending.BlendEnabled) {
				graphics::BlendState& state = RasterState.Blending;
				glEnable(GL_BLEND);
				glBlendEquationSeparate(*state.RgbBlendFunc, *state.AlphaBlendFunc);
				glBlendFuncSeparate(*state.SrcRgb, *state.DstRgb, *state.SrcAlpha, *state.DstAlpha);
			} else {
				glDisable(GL_BLEND);
			}

			// New in tutorial 07
			int slot = 0;
			for (auto& kvp : myTextures) {
				if (kvp.second.Sampler != nullptr)
					kvp.second.Sampler->Bind(slot);
				else
					graphics::TextureSampler::Unbind(slot);

				kvp.second.Texture->Bind(slot);
				myShader->SetUniform(kvp.first, slot);
				slot++;
			}
		}

		Material::Sptr Material::Clone() {
			Sptr result = std::make_shared<Material>(*this);

			for (auto& kvp : myMat4s) { result->Set(kvp.first, kvp.second); }
			for (auto& kvp : myVec4s) { result->Set(kvp.first, kvp.second); }
			for (auto& kvp : myVec3s) { result->Set(kvp.first, kvp.second); }
			for (auto& kvp : myVec2s) { result->Set(kvp.first, kvp.second); }
			for (auto& kvp : myFloats) { result->Set(kvp.first, kvp.second); }
			for (auto& kvp : myInts) { result->Set(kvp.first, kvp.second); }
			for (auto& kvp : myTextures) { result->Set(kvp.first, kvp.second.Texture, kvp.second.Sampler); }
			
			return result;
		}
		
	}
}