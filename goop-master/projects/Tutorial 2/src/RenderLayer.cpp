#include "RenderLayer.h"
#include <florp\game\SceneManager.h>
#include <florp\game\RenderableComponent.h>
#include <florp\app\Timing.h>
#include <florp\game\Transform.h>
#include <florp\game\CameraComponent.h>

typedef florp::game::RenderableComponent Renderable;

void sortRenderers(entt::registry& reg) {
	// We sort our mesh renderers based on material properties
	// This will group all of our meshes based on shader first, then material second
	reg.sort<florp::game::RenderableComponent>([](const florp::game::RenderableComponent& lhs, const florp::game::RenderableComponent& rhs) {
		if (rhs.Material == nullptr || rhs.Mesh == nullptr)
			return false;
		else if (lhs.Material == nullptr || lhs.Mesh == nullptr)
			return true;
		else if (lhs.Material->RasterState.Blending.BlendEnabled & !rhs.Material->RasterState.Blending.BlendEnabled)
			return false;
		else if (!lhs.Material->RasterState.Blending.BlendEnabled & rhs.Material->RasterState.Blending.BlendEnabled)
			return true;
		else if (lhs.Material->GetShader() != rhs.Material->GetShader())
			return lhs.Material->GetShader() < rhs.Material->GetShader();
		else
			return lhs.Material < rhs.Material;
		});
}

void ctorSort(entt::entity, entt::registry& ecs, const Renderable& r) {
	sortRenderers(ecs);
}
void dtorSort(entt::entity, entt::registry& ecs) {
	sortRenderers(ecs);
}

void RenderLayer::OnSceneEnter() {
	CurrentRegistry().on_construct<Renderable>().connect<&::ctorSort>();
	CurrentRegistry().on_destroy<Renderable>().connect<&::dtorSort>();
}

void RenderLayer::PreRender() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void RenderLayer::Render()
{
	auto& ecs = CurrentRegistry();

	florp::game::Material::Sptr material = nullptr;
	florp::graphics::Shader::Sptr boundShader = nullptr;

	florp::game::Transform camTransform;
	florp::game::CameraComponent cam;

	ecs.view<florp::game::CameraComponent>().each([&](auto entity, auto& camComponent) {
		camTransform = ecs.get<florp::game::Transform>(entity);
		cam = camComponent;
	});

	glm::vec3 postion = camTransform.GetLocalPosition();
	glm::mat4 viewMatrix = glm::inverse(camTransform.GetWorldTransform());
	glm::mat4 viewProjection = cam.Projection * viewMatrix;

	// A view will let us iterate over all of our entities that have the given component types
	auto view = ecs.view<Renderable>();

	for (const auto& entity : view) {

		// Get our shader
		const Renderable& renderer = ecs.get<Renderable>(entity);

		// Early bail if mesh is invalid
		if (renderer.Mesh == nullptr || renderer.Material == nullptr)
			continue;

		// If our shader has changed, we need to bind it and update our frame-level uniforms
		if (renderer.Material->GetShader() != boundShader) {
			boundShader = renderer.Material->GetShader();
			boundShader->Use();
			boundShader->SetUniform("a_CameraPos", postion);
			boundShader->SetUniform("a_Time", florp::app::Timing::GameTime);
		}

		// If our material has changed, we need to apply it to the shader
		if (renderer.Material != material) {
			material = renderer.Material;
			material->Apply();
		}

		// We'll need some info about the entities position in the world
		const florp::game::Transform& transform = ecs.get_or_assign<florp::game::Transform>(entity);

		// Our normal matrix is the inverse-transpose of our object's world rotation
		glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(transform.GetWorldTransform())));

		// Update the MVP using the item's transform
		boundShader->SetUniform(
			"a_ModelViewProjection",
			viewProjection *
			transform.GetWorldTransform());

		// Update the model matrix to the item's world transform
		boundShader->SetUniform("a_Model", transform.GetWorldTransform());

		// Update the model matrix to the item's world transform
		boundShader->SetUniform("a_NormalMatrix", normalMatrix);

		// Draw the item
		renderer.Mesh->Draw();
	}
}
