#pragma once
#include <entt.hpp>
#include "IBehaviour.h"
#include "Logging.h"

namespace florp {
	namespace game {

		class Scene {
		public:
			Scene() = default;
			virtual ~Scene() = default;
			
			void Load();
			void Unload();

			/*
			 * Gets this scene's ENTT registry
			 */
			entt::registry& Registry() { return myRegistry; }

			/*
			 * Creates a new entity with the default components
			 */
			entt::entity CreateEntity();

			/*
			 * Adds a component of type T to the given entity
			 * @param T The type of component to add
			 * @param TArgs The type arguments to pass to the constructor of the component
			 * @param e The entity to add the component to
			 * @param args The arguments to pass to the component's constructor
			 * @returns A reference to the component added to the entity
			 */
			template <typename T, typename ... TArgs>
			T& Assign(entt::entity e, TArgs&&... args) {
				return myRegistry.assign<T>(e, std::forward<TArgs>(args)...);
			}
			/*
			 * Gets a component of type T from the given entity
			 * @param T The type of component to get
			 * @param e The entity to get the component from
			 * @returns A reference to the component fetched from the entity
			 */
			template <typename T>
			T& Get(entt::entity e) {
				if (myRegistry.has<T>(e)) {
					return myRegistry.get<T>(e);
				}
				else {
					LOG_ASSERT(false, "The entity does not contain a component of that type!");
					return *(T*)nullptr;
				}
			}
			/*
			 * Gets a component of type T from the given entity, or creates it with the parameters if it does not exist
			 * @param T The type of component to get
			 * @param TArgs The type arguments to pass to the constructor of the component
			 * @param e The entity to get the component from
			 * @param args The arguments to pass to the component's constructor if the entity does not have that component attached
			 * @returns A reference to the component fetched from or added to the entity
			 */
			template <typename T, typename ... TArgs>
			T& GetOrAssign(entt::entity e, TArgs&&... args) {
				return myRegistry.get_or_assign<T>(e, std::forward<TArgs>(args)...);
			}

			/*
			 * Adds a behaviour to a given entity
			 * @param T The type of behaviour to add
			 * @param TArgs The type parameters for the arguments to forward to the constructor
			 * @param e The entity to add the behaviour to
			 * @param args The arguments to forward to the behaviour's constructor
			 */
			template <typename T, typename ... TArgs, typename = typename std::enable_if<std::is_base_of<IBehaviour, T>::value>::type>
			void AddBehaviour(entt::entity e, TArgs&&... args) {
				BehaviourBinding::Bind<T>(myRegistry, e, std::forward<TArgs>(args)...);
			}
			/*
			 * Checks whether the given entity has a behaviour of a given type
			 * @param T The type of behaviour to check for
			 * @param e The entity to check
			 * @returns True if the entity contains a behaviour of type T, false if otherwise
			 */
			template <typename T, typename = typename std::enable_if<std::is_base_of<IBehaviour, T>::value>::type>
			bool HasBehaviour(entt::entity e) {
				return BehaviourBinding::Has<T>(myRegistry, e);
			}
			/*
			 * Gets the behaviour with the given type from the entity, or nullptr if none exists
			 * @param T The type of behaviour to check for
			 * @param entity The entity to search
			 * @returns The behaviour of type T that is attached to entity, or nullptr if no behaviour of that type is attached
			 */
			template <typename T, typename = typename std::enable_if<std::is_base_of<IBehaviour, T>::value>::type>
			std::shared_ptr<T> GetBehaviour(entt::entity e) {
				return BehaviourBinding::Get<T>(myRegistry, e);
			}

			const std::string& GetName() const { return myName; }
			void SetName(const std::string& name) { myName = name; }

		protected:
			entt::registry myRegistry;
			std::string myName;
		};
		
	}
}
