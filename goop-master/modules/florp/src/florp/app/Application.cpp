#include "florp/app/Application.h"
#include "Logging.h"
#include "florp/app/Timing.h"

namespace florp {
	namespace app {

		void Application::OnWindowSizeChanged(Window& window, uint32_t width, uint32_t height, void* userPointer) {
			if (width > 0 && height > 0) {
				for (ApplicationLayer* layer : mySingleton->myLayers) {
					layer->OnWindowResize(width, height);
				}
			}			
		}
		
		Application* Application::mySingleton = nullptr;
		
		Application::Application() {
			Logger::Init();

			LOG_ASSERT(mySingleton == nullptr, "Another application is already running!");
			mySingleton = this;

			myWindow = std::make_shared<Window>();
			myWindow->OnSizeChanged = OnWindowSizeChanged;
		}
		
		Application::~Application() {
			for (ApplicationLayer* layer : myLayers){
				delete layer;
			}
			Logger::Uninitialize();
		}

		void Application::Run() {			
			// Initializes the client application, handles registering all the application layers
			Initialize();

			// Initialize the timers to default
			float fixedStep = 0.0f;
			float frameStep = 0.0f;
				
			// Perform initializations for all application layers 
			for (ApplicationLayer* layer : myLayers) {
				layer->Initialize();
			}
			
			Timing::LastFrame = static_cast<float>(glfwGetTime());
			Timing::TimeSinceLoad = 0.0f;

			// Run the application as long as the main window is open
			while(!myWindow->GetShouldClose()) {
				// Poll for window events
				myWindow->Poll();

				// Update the relevant timers
				Timing::GameTime = static_cast<float>(glfwGetTime());
				Timing::DeltaTime = Timing::GameTime - Timing::LastFrame;
				Timing::TimeSinceLoad += Timing::DeltaTime;

				// Advance the fixed and frame step timers
				fixedStep += Timing::DeltaTime;
				frameStep += Timing::DeltaTime;

				// Update all the application layers (variable rate updates, run as fast as possible)
				for (ApplicationLayer* layer : myLayers) {
					if (layer->IsEnabled) layer->Update();
				}
				// Perform the late update for all the application layers (variable rate updates, run as fast as possible)
				for (ApplicationLayer* layer : myLayers) {
					if (layer->IsEnabled) layer->LateUpdate();
				}

				// Perform the fixed update if enough time has elapsed
				if (fixedStep > Timing::FixedTimeStep) {
					fixedStep -= Timing::FixedTimeStep;
					// Fixed update all the application layers
					for(ApplicationLayer* layer : myLayers) {
						if (layer->IsEnabled) layer->FixedUpdate();
					}
				}

				// Perform the render step if enough time has passed
				if (frameStep > Timing::FrameTimeStep) {
					frameStep -= Timing::FrameTimeStep;
					// Pre-render for all layers
					for (ApplicationLayer* layer : myLayers) {
						if (layer->IsEnabled) layer->PreRender();
					}
					// Render all the application layers
					for (ApplicationLayer* layer : myLayers) {
						if (layer->IsEnabled) layer->Render();
					}
					// Post-render for all layers
					for (ApplicationLayer* layer : myLayers) {
						if (layer->IsEnabled) layer->PostRender();
					}
					
					// Pre GUI render for all layers
					for (ApplicationLayer* layer : myLayers) {
						if (layer->IsEnabled) layer->PreGUI();
					}
					// Render the GUI for all the application layers
					for (ApplicationLayer* layer : myLayers) {
						if (layer->IsEnabled) layer->RenderGUI();
					}
					// Post GUI render for all layers
					for (ApplicationLayer* layer : myLayers) {
						if (layer->IsEnabled) layer->PostGUI();
					}

					// Present the frame
					myWindow->SwapBuffers();
				}

				// Update our previous frame timer
				Timing::LastFrame = Timing::GameTime;		
			}

			 // Perform the shutdown behaviors for all application layers 
			for (ApplicationLayer* layer : myLayers) {
				layer->Shutdown();
			}

			// Shuts down the client application, allowing client apps to provide shutdown behaviour
			Shutdown();

			mySingleton = nullptr;
		}

		void Application::Close() {
			myWindow->Close();
		}

		void Application::AddLayer(ApplicationLayer* layer) {
			myLayers.push_back(layer);
		}

		void Application::EachLayer(std::function<void(ApplicationLayer*)> callback) {
			if (this != nullptr) {
				for (auto* layer : myLayers) {
					callback(layer);
				}
			}
		}
	}
}
