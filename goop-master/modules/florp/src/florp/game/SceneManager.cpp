#include "florp/game/SceneManager.h"
#include "Logging.h"
#include "florp/app/Application.h"

namespace florp {
	namespace game {
		Scene* SceneManager::_CurrentScene = nullptr;
		std::unordered_map<std::string, Scene*> SceneManager::_KnownScenes;
		entt::registry SceneManager::Prefabs;

		Scene* SceneManager::Current() {
			return _CurrentScene;
		}

		bool SceneManager::HasScene(const std::string& name) {
			auto it = _KnownScenes.find(name);
			if (it != _KnownScenes.end()) {
				return true;
			}
			return false;
		}

		Scene* SceneManager::Get(const std::string& name) {
			if (!HasScene(name))
				return nullptr;
			else
				return _KnownScenes[name];
		}

		bool SceneManager::SetCurrentScene(const std::string& name) {
			auto it = _KnownScenes.find(name);
			if (it != _KnownScenes.end()) {
				if (_CurrentScene != it->second) {
					if (_CurrentScene != nullptr) {
						app::Application::Get()->EachLayer([](app::ApplicationLayer* layer) { layer->OnSceneExit(); });
						_CurrentScene->Unload();
					}
					_CurrentScene = it->second;
					_CurrentScene->Load();
					app::Application::Get()->EachLayer([](app::ApplicationLayer* layer) { layer->OnSceneEnter(); });
				}
				return true;
			}
			return false;
		}

		Scene* SceneManager::RegisterScene(const std::string& name, Scene* scene) {
			LOG_ASSERT(!HasScene(name), "A scene with that name already exists!");

			if (scene == nullptr)
				scene = new Scene();

			_KnownScenes[name] = scene;
			return scene;
		}

		SceneManager::SceneIterator SceneManager::Each() {
			return SceneIterator();
		}

		void SceneManager::DestroyScenes() {
			for (auto& kvp : _KnownScenes) {
				delete kvp.second;
			}

			_KnownScenes.clear();
			_CurrentScene = nullptr;
		}
	}
}
