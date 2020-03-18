#pragma once
#include <unordered_map>
#include <entt.hpp>
#include "Scene.h"

namespace florp {
	namespace game {
		
		/*
		 * Manages all of the scenes in our game 
		 */
		class SceneManager {
		public:
			/*
			 * Allows us to iterate over all the scenes in this manager
			 */
			struct SceneIterator {
				typedef std::unordered_map<std::string, Scene*>::iterator iterator;
				typedef std::unordered_map<std::string, Scene*>::const_iterator const_iterator;
				iterator begin() { return SceneManager::_KnownScenes.begin(); }
				iterator end() { return SceneManager::_KnownScenes.end(); }
				const_iterator begin() const { return SceneManager::_KnownScenes.begin(); }
				const_iterator end() const { return SceneManager::_KnownScenes.end(); }
			};

			/*
			 * Gets the current game scene
			 */
			static Scene* Current();
			/*
			 * Sets the current game scene to the one with the given name. If no scene with the name exists, no changes
			 * take place and false is returned
			 * @param name The name of the scene to switch to
			 * @returns True if the scene has changed, false if otherwise
			 */
			static bool SetCurrentScene(const std::string& name);

			/*
			 * Checks whether a scene with a given name exists
			 * @param name The name to search for
			 * @returns True if a scene with the name exists, false if otherwise
			 */
			static bool HasScene(const std::string& name);
			/*
			 * Gets the scene with the given name
			 * @param name The name of the scene to get
			 * @returns The scene with the given name, or nullptr if there is no scene by that name
			 */
			static Scene* Get(const std::string& name);

			/*
			 * Registers a new scene with the given name. If no scene is given, an empty one will be created
			 * @param name The name of the scene to add
			 * @param scene The scene to add, or nullptr to create a new scene
			 */
			static Scene* RegisterScene(const std::string& name, Scene* scene = nullptr);

			/*
			 * Gets a scene iterator which allows us to iterate over all available scenes
			 */
			static SceneIterator Each();

			/*
			 * Destroy all scenes that have been registered
			 */
			static void DestroyScenes();

			// An ENTT registry for template entities which can be instantiated later
			static entt::registry Prefabs;

		private:
			static Scene* _CurrentScene;
			static std::unordered_map<std::string, Scene*> _KnownScenes;
		};

		// We can make some macros to shorten our calls
		// Gets the current scene
		#define CurrentScene() florp::game::SceneManager::Current()
		// Gets the current scene's registry
		#define CurrentRegistry() florp::game::SceneManager::Current()->Registry()
		// Gets the registry associated with a given scene
		#define GetRegistry(name) florp::game::SceneManager::Get(name)->Registry()
		
	}
}
