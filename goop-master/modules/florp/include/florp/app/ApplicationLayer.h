#pragma once
#include <cstdint>

namespace  florp {
	namespace app {

		/*
		 * Base class for application layers, which provide a simple way to add new systems to an application
		 */
		class ApplicationLayer
		{
		public:
			virtual ~ApplicationLayer() = default;

			/*
			 * Allows the layer to handle window resizing
			 * @param width The new width of the window
			 * @param height The new height of the window
			 */
			virtual void OnWindowResize(uint32_t width, uint32_t height) {}
			
			/*
			 * Initializes this application layer, creating all the required resources
			 */
			virtual void Initialize() {};
			/*
			 * Handles shutting down this application layer, freeing all resources
			 */
			virtual void Shutdown() {};

			/*
			 * Allows the application layer to perform variable rate updates (invoked once every fixed timestep)
			 */
			virtual void Update() {};
			/*
			 * Allows the application layer to perform a variable rate update after all other layers have performed their own updates
			 */
			virtual void LateUpdate() {};
			/*
			 * Allows the application to update it's logic at a fixed rate
			 */
			virtual void FixedUpdate() {};

			/*
			 * Allows the application layer to perform events before all rendering
			 */
			virtual void PreRender() {};
			/*
			 * Allows the application layer to perform rendering
			 */
			virtual void Render() {};
			/*
			 * Allows the application layer to perform events after rendering is finished
			 */
			virtual void PostRender() {};

			/*
			 * Allows the application layer to perform events before all GUI rendering
			 */
			virtual void PreGUI() {};
			/*
			 * Allows the application layer to perform GUI rendering, occurs after all layers have Rendered
			 */
			virtual void RenderGUI() {};
			/*
			 * Allows the application layer to perform events after all GUI rendering
			 */
			virtual void PostGUI() {};

			/*
			 * Allows the application layer to hook into scene changes
			 */
			virtual void OnSceneExit(){}
			/*
			 * Allows the application layer to hook into scene changes
			 */
			virtual void OnSceneEnter() {}

			/*
			 * Whether or not this application layer is enabled
			 */
			bool IsEnabled;

		protected:
			ApplicationLayer() : IsEnabled(true) {};
		};

	}
}
