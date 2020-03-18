#include "LightingLayer.h"
#include <florp\game\SceneManager.h>
#include <florp\game\Transform.h>
#include <florp\game\RenderableComponent.h>
#include <ShadowLight.h>
#include "florp/app/Application.h"
#include "FrameState.h"
#include <imgui.h>

void LightingLayer::OnWindowResize(uint32_t width, uint32_t height) {
	myAccumulationBuffer->Resize(width, height);
}

void LightingLayer::Initialize() {
	florp::app::Application* app = florp::app::Application::Get();
	
	using namespace florp::graphics;

	// We'll set our ambient light to be some very small amount (this will be for the entire scene)
	myAmbientLight = glm::vec3(0.05f);
	
	// The normal shader will handle depth map generation for shadow casting lights (note that we'll just use the default, fallback fragment shader)
	myShader = std::make_shared<Shader>();
	myShader->LoadPart(ShaderStageType::VertexShader, "shaders/simple.vs.glsl");
	myShader->Link();

	// The masked shader will be used if we have a light that will mask off areas to not cast shadows (for instance, behind a grate)
	myMaskedShader = std::make_shared<Shader>();
	myMaskedShader->LoadPart(ShaderStageType::VertexShader, "shaders/simple.vs.glsl");  
	myMaskedShader->LoadPart(ShaderStageType::FragmentShader, "shaders/shadow_masked.fs.glsl");  
	myMaskedShader->Link();

	// The shadow composite shader will handle adding shadow casting and projector lights to our accumulation buffer
	myShadowComposite = std::make_shared<Shader>();
	myShadowComposite->LoadPart(ShaderStageType::VertexShader, "shaders/post/post.vs.glsl");
	myShadowComposite->LoadPart(ShaderStageType::FragmentShader, "shaders/post/shadow_post.fs.glsl");
	myShadowComposite->Link();

	// The final composite shader will handle applying the lighting, and doing our HDR correction for later passes
	myFinalComposite = std::make_shared<Shader>();
	myFinalComposite->LoadPart(ShaderStageType::VertexShader, "shaders/post/post.vs.glsl");
	myFinalComposite->LoadPart(ShaderStageType::FragmentShader, "shaders/post/lighting_composite.fs.glsl");
	myFinalComposite->Link();
	myFinalComposite->SetUniform("a_Exposure", 1.0f);
	
	// NOTE: Our accumulation buffer will be a floating point buffer, to allow us to do some HDR lighting (we'll map it back to to 0-1 range for further processing)
	static_assert(false);
	
	// Our accumulation buffer will be a floating-point buffer, so we can do some HDR lighting effects
	RenderBufferDesc mainColor = RenderBufferDesc();
	mainColor.ShaderReadable = true;
	mainColor.Attachment = RenderTargetAttachment::Color0;
	mainColor.Format = RenderTargetType::ColorRgb16F;


	// We'll use one buffer to accumulate all the lighting
	myAccumulationBuffer = std::make_shared<FrameBuffer>(app->GetWindow()->GetWidth(), app->GetWindow()->GetHeight());
	myAccumulationBuffer->AddAttachment(mainColor);
	myAccumulationBuffer->Validate();
	myAccumulationBuffer->SetDebugName("Intermediate");

	// Create our fullscreen quad (just like in PostLayer)
	{
		float vert[] = {
			-1.0f, -1.0f,  0.0f, 0.0f,
			 1.0f, -1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f,  0.0f, 1.0f,
			 1.0f,  1.0f,  1.0f, 1.0f
		};
		uint32_t indices[] = {
			0, 1, 2,
			1, 3, 2
		};
		florp::graphics::BufferLayout layout = {
			{ "inPosition", florp::graphics::ShaderDataType::Float2 },
			{ "inUV",       florp::graphics::ShaderDataType::Float2 }
		};

		myFullscreenQuad = std::make_shared<florp::graphics::Mesh>(vert, 4, layout, indices, 6);
	}
}

void LightingLayer::PreRender()
{ 
	using namespace florp::game;
	using namespace florp::graphics;

	auto& ecs = CurrentRegistry();

	// We'll only handle stuff if we actually have a shadow casting light in the scene
	auto view = ecs.view<ShadowLight>();
	if (view.size() > 0) {
		// We'll make sure depth testing and culling are enabled
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT); // enable front face culling

		// Iterate over all the shadow casting lights
		Shader::Sptr shader = nullptr;
		ecs.view<ShadowLight>().each([&](auto entity, ShadowLight& light) {
			// Get the light's transform
			const Transform& lightTransform = ecs.get<Transform>(entity);

			// Select which shader to use depending on if the light has a mask or not
			if (light.Mask == nullptr) {
				shader = myShader;
			}
			else {
				shader = myMaskedShader;
				light.Mask->Bind(0);
			}
			// Use the shader, and tell it what our output resolution is
			shader->Use();
			shader->SetUniform("a_OutputResolution", (glm::vec2)light.ShadowBuffer->GetSize());

			// Bind, viewport, and clear
			light.ShadowBuffer->Bind();
			glViewport(0, 0, light.ShadowBuffer->GetWidth(), light.ShadowBuffer->GetHeight());
			glClear(GL_DEPTH_BUFFER_BIT);

			// TODO: We will need to implement rendering our objects!
			static_assert(false);

			// Unbind so that we can use the texture later
			light.ShadowBuffer->UnBind();
		});

		glCullFace(GL_BACK); // enable back face culling
	}
}

void LightingLayer::PostRender() {
	// You should read through this function and understand what's going on,
	// The important thing is that we are accumulating all of our lighting calculations into
	// a single floating point buffer, and combining it with our main output at the end
	// Note that we are updating the values in the main buffer itself!
	static_assert(false);

	// Bind and clear our lighting accumulation buffer
	myAccumulationBuffer->Bind();
	glClearColor(myAmbientLight.r, myAmbientLight.g, myAmbientLight.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Disable Depth testing, and enable additive blending
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	   
	// Do our light post processing
	PostProcessShadows();
	PostProcessLights();

	// Unbind the accumulation buffer so we can blend it with the main scene
	myAccumulationBuffer->UnBind();

	// Disable blending, we will overwrite the contents now
	glDisable(GL_BLEND);
	
	// We grab the application singleton to get the size of the screen
	florp::app::Application* app = florp::app::Application::Get();
	auto& ecs = CurrentRegistry();
	
	// We'll get the back buffer from the frame state
	const AppFrameState& state = ecs.ctx<AppFrameState>();
	FrameBuffer::Sptr mainBuffer = state.Current.Output;
	
	// Set the main buffer as the output again
	mainBuffer->Bind();
	// We'll use an additive shader for now, this should be a multiply with the albedo of the scene
	myFinalComposite->Use();
	// We'll combine the GBuffer color and our lighting contributions
	mainBuffer->Bind(1, RenderTargetAttachment::Color0);
	myAccumulationBuffer->Bind(2);
	// Render the quad
	myFullscreenQuad->Draw();
	// Unbind main buffer to perform multisample blitting
	mainBuffer->UnBind();   
}

void LightingLayer::RenderGUI()
{
	// We'll put all the lighting stuff into it's own ImGUI window
	ImGui::Begin("Lighting Settings");

	// For now, we'll just have a slider to adjust our exposure
	static float exposure = 1.0f;
	if (ImGui::DragFloat("Exposure", &exposure, 0.1f, 0.1f, 10.0f)) {
		myFinalComposite->SetUniform("a_Exposure", exposure);
	}
	// We'll have a color picker for the ambient light color
	ImGui::ColorEdit3("Ambient", &myAmbientLight.x);
	
	ImGui::End();
}

void LightingLayer::PostProcessShadows() {
	// We grab the application singleton to get the size of the screen
	florp::app::Application* app = florp::app::Application::Get();
	auto& ecs = CurrentRegistry();

	// We'll get the back buffer from the frame state
	const AppFrameState& state = ecs.ctx<AppFrameState>();
	FrameBuffer::Sptr mainBuffer = state.Current.Output;

	// We can extract our near and far plane by reversing the projection calculation
	float m22 = state.Current.Projection[2][2];
	float m32 = state.Current.Projection[3][2];
	float nearPlane = (2.0f * m32) / (2.0f * m22 - 2.0f);
	float farPlane = ((m22 - 1.0f) * nearPlane) / (m22 + 1.0);

	// TODO: Set up uniforms and bind the gBuffer to the shader
	static_assert(false);
	

	// Iterate over all the ShadowLights in the scene
	auto view = CurrentRegistry().view<ShadowLight>();
	if (view.size() > 0) {
		view.each([&](auto entity, ShadowLight& light) {

			// TODO: Handle post-processing our shadow casting lights
			static_assert(false);
			
		});
	}
}

void LightingLayer::PostProcessLights() {
	// TODO: Deferred lighting
}
