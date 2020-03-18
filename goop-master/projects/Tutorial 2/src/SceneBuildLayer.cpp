#include "SceneBuildLayer.h"
#include "florp/game/SceneManager.h"
#include "florp/game/RenderableComponent.h"
#include <florp\graphics\MeshData.h>
#include <florp\graphics\MeshBuilder.h>
#include <florp\graphics\ObjLoader.h>
#include <florp\game\CameraComponent.h>

#include <glm/gtc/matrix_transform.hpp>
#include <florp\game\Transform.h>

void SceneBuilder::Initialize()
{
	using namespace florp::game;
	using namespace florp::graphics;
	
	auto* scene = SceneManager::RegisterScene("main");
	SceneManager::SetCurrentScene("main");

	MeshData data = ObjLoader::LoadObj("monkey.obj", glm::vec4(1.0f));

	Shader::Sptr shader = std::make_shared<Shader>();
	shader->LoadPart(ShaderStageType::VertexShader, "shaders/lighting.vs.glsl");
	shader->LoadPart(ShaderStageType::FragmentShader, "shaders/blinn-phong.fs.glsl");
	shader->Link();

	Material::Sptr mat = std::make_shared<Material>(shader);
	mat->Set("a_LightPos", { 2, 0, 4 });
	mat->Set("a_LightColor", { 1.0f, 1.0f, 1.0f });
	mat->Set("a_AmbientColor", { 1.0f, 1.0f, 1.0f });
	mat->Set("a_AmbientPower", 0.1f);
	mat->Set("a_LightSpecPower", 0.5f);
	mat->Set("a_LightShininess", 256.0f);
	mat->Set("a_LightAttenuation", 1.0f / 100.0f);

	{
		entt::entity test = scene->CreateEntity();
		//scene->AddBehaviour<SampleBehaviour>(test, "Hello world!");
		RenderableComponent& renderable = scene->Registry().assign<RenderableComponent>(test);
		renderable.Mesh = MeshBuilder::Bake(data);
		renderable.Material = mat;
		Transform& t = scene->Registry().get<Transform>(test);
		t.SetPosition(glm::vec3(0, 0, -10));
	}

	{
		entt::entity camera = scene->CreateEntity();
		CameraComponent& cam = scene->Registry().assign<CameraComponent>(camera);
		cam.Projection = glm::perspective(glm::radians(60.0f), 1.0f, 0.01f, 1000.0f);
		Transform& t = scene->Registry().get<Transform>(camera);
		t.SetPosition(glm::vec3(0.0f, 0.0f, -5.0f));
	}
}
