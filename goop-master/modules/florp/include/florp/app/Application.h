#pragma once
#include "Window.h"
#include "ApplicationLayer.h"
#include <functional>

namespace florp {
	namespace app {

		/*
		 * Provides a base class for client applications
		 */
		class Application
		{
		public:
			/*
			 * Default constructor for applications, initializes logging and creates a new window
			 */
			Application();
			virtual ~Application();

			/*
			 * Runs the application, and handles updating and rendering all the application layers.
			 * This is a blocking function
			 */
			void Run();
			/*
			 * Closes the application
			 */
			void Close();

			/*
			 * Adds a new layer to this application
			 * @param layer The application layer to add
			 */
			void AddLayer(ApplicationLayer* layer);
						
			/*
			 * Creates and adds a new layer to this application
			 * @param T The type of layer to create and add
			 * @param layer The application layer to add
			 */
			template <typename T, typename = typename std::enable_if<std::is_base_of<ApplicationLayer, T>::value>::type>
			void AddLayer() {
				AddLayer(new T());
			}

			/*
			 * Iterates over all layers and calls a function on them
			 */
			void EachLayer(std::function<void(ApplicationLayer*)> callback);

			/*
			 * Gets the main window associated with the application
			 */
			const Window::Sptr& GetWindow() const { return myWindow; }
			
			/*
			 * Gets the currently running application
			 */
			static Application* Get() { return mySingleton; }


		protected:
			// Stores all of the layers in this application, each layer will be deleted upon app closing
			std::vector<ApplicationLayer*> myLayers;
			// Stores the window that will render this application
			Window::Sptr myWindow;

			/*
			 * Allows a client application to initialize it's application layers
			 */
			virtual void Initialize() {};
			/*
			 * Allows a client to implement a shutdown behaviour
			 */
			virtual void Shutdown() {};

			static Application* mySingleton;

		private:
			static void OnWindowSizeChanged(Window& window, uint32_t width, uint32_t height, void* userPointer);
		};
		
	}
}
