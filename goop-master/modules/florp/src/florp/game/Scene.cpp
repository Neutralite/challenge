#include "florp/game/Scene.h"
#include "florp/game/Transform.h"

namespace florp {
	namespace game {
		
		void Scene::Load() {
			
		}

		void Scene::Unload() {
			
		}

		entt::entity Scene::CreateEntity() {
			entt::entity result = myRegistry.create();
			myRegistry.assign<Transform>(result);
			return  result;
		}
	}
}
