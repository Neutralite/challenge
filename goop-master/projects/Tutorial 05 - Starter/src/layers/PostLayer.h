#pragma once
#include "florp/app/ApplicationLayer.h"
#include "florp/app/Window.h"
#include "FrameBuffer.h"
#include "florp/graphics/Mesh.h"
#include "florp/graphics/Shader.h"

class PostLayer : public florp::app::ApplicationLayer
{
public:
	PostLayer();
	virtual void OnWindowResize(uint32_t width, uint32_t height) override;
	virtual void PostRender() override;
	virtual void Update() override;

protected:
	florp::graphics::Mesh::Sptr myFullscreenQuad;

	struct PostPass {
		typedef std::shared_ptr<PostPass> Sptr;

		florp::graphics::Shader::Sptr Shader;
		FrameBuffer::Sptr             Output;
		struct Input {
			Sptr                      Pass;
			RenderTargetAttachment    Attachment = RenderTargetAttachment::Color0;
			bool                      UsePrevFrame = false;
		};
		std::vector<Input>            Inputs;

		float                         ResolutionMultiplier = 1.0f;
		bool                          Enabled = true;
	};
	std::vector<PostPass::Sptr> myPasses;
	std::unordered_map<florp::app::Key, std::vector<PostPass::Sptr>> myToggleInputs;
	
	PostPass::Sptr __CreatePass(const char* fragmentShader, float scale = 1.0f) const;

};
