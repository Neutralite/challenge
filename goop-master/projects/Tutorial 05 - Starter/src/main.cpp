#include "florp/app/Application.h"
#include "florp/game/BehaviourLayer.h"
#include "florp/game/ImGuiLayer.h"
#include "layers/SceneBuildLayer.h"
#include "layers/RenderLayer.h"
#include "layers/PostLayer.h"
#include "layers/AudioLayer.h"
#include "layers/LightingLayer.h"

int main()
{
	{
		// Create our application
		florp::app::Application* app = new florp::app::Application();

		// Set up our layers
		app->AddLayer<florp::game::BehaviourLayer>();
		app->AddLayer<florp::game::ImGuiLayer>();
		app->AddLayer<AudioLayer>();
		app->AddLayer<SceneBuilder>();
		app->AddLayer<RenderLayer>();
		// Note that the Lighting layer MUST be placed between the RenderLayer and the PostLayer
		static_assert(false); 
		app->AddLayer<LightingLayer>(); 
		app->AddLayer<PostLayer>();

		app->Run();

		delete app;
	}
		
	return 0;
} 
