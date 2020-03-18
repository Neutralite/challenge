#include "glad/glad.h"
#include "florp/app/Window.h"
#include "Logging.h"

namespace florp {
	namespace app {
		/*
			Handles debug messages from OpenGL
			https://www.khronos.org/opengl/wiki/Debug_Output#Message_Components
			@param source    Which part of OpenGL dispatched the message
			@param type      The type of message (ex: error, performance issues, deprecated behavior)
			@param id        The ID of the error or message (to distinguish between different types of errors, like nullref or index out of range)
			@param severity  The severity of the message (from High to Notification)
			@param length    The length of the message
			@param message   The human readable message from OpenGL
			@param userParam The pointer we set with glDebugMessageCallback (should be the game pointer)
		*/
		void GlDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
			std::string sourceTxt;
			switch (source) {
			case GL_DEBUG_SOURCE_API: sourceTxt = "DEBUG"; break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceTxt = "WINDOW"; break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceTxt = "SHADER"; break;
			case GL_DEBUG_SOURCE_THIRD_PARTY: sourceTxt = "THIRD PARTY"; break;
			case GL_DEBUG_SOURCE_APPLICATION: sourceTxt = "APP"; break;
			case GL_DEBUG_SOURCE_OTHER: default: sourceTxt = "OTHER"; break;
			}
			switch (severity) {
			case GL_DEBUG_SEVERITY_LOW:          LOG_INFO("[{}] {}", sourceTxt, message); break;
			case GL_DEBUG_SEVERITY_MEDIUM:       LOG_WARN("[{}] {}", sourceTxt, message); break;
			case GL_DEBUG_SEVERITY_HIGH:         LOG_ERROR("[{}] {}", sourceTxt, message); break;
				#ifdef LOG_GL_NOTIFICATIONS
			case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_INFO("[{}] {}", sourceTxt, message); break;
				#endif
			default: break;
			}
		}
		
		
		bool Window::IsInitialized = false;
		uint32_t Window::WindowCount = 0;

		void Window::ResizeProxy(GLFWwindow* window, int width, int height) {
			if (width > 0 && height > 0) {
				Window* fWindow = (Window*)glfwGetWindowUserPointer(window);
				if (fWindow) {
					fWindow->__InternalResizeEvent(width, height);
				}
			}
		}

		Window::Window(uint32_t width, uint32_t height, const std::string& title, Renderer renderer)
		{
			// Our static constructor
			WindowCount++;
			if (!IsInitialized) {
				__Initialize();
			}

			// Initialize the callbacks to be null
			OnSizeChanged = nullptr;
			OnKeyChange   = nullptr;
			OnMouseButton = nullptr;
			isMinimized   = false;
			myUserPointer = nullptr;
			memset(myKeyStates, 0, sizeof(myKeyStates));
			memset(myMouseStates, 0, sizeof(myMouseStates));

			// Store the window parameters
			myWidth  = width;
			myHeight = height;
			myTitle  = title;
				
			// Create a new GLFW window
			myWindow = glfwCreateWindow(myWidth, myHeight, myTitle.c_str(), nullptr, nullptr);

			// Tie our game to our window, so we can access it via callbacks
			glfwSetWindowUserPointer(myWindow, this);

			// Set our window resized callback
			glfwSetWindowSizeCallback(myWindow, ResizeProxy);

			// We want GL commands to be executed for our window, so we make our window's context the current one
			glfwMakeContextCurrent(myWindow);

			// Spit frames out as fast as possible (no v-sync)
			// Fast as FUC (Fast as Framework Updates Canvas)
			glfwSwapInterval(0);

			// In the future, we will initialize our renderer depending on what Renderer we have selected
			switch(renderer) {
				case Renderer::OpenGL:
					// Let glad know what function loader we are using (will call gl commands via glfw)
					if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
						std::cout << "Failed to initialize Glad" << std::endl;
						throw std::runtime_error("Failed to initialize GLAD");
					}
					glEnable(GL_DEBUG_OUTPUT);
					glDebugMessageCallback(GlDebugMessage, this);
					glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

					glEnable(GL_DEPTH_TEST);
					glEnable(GL_CULL_FACE);
					glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
					break;
				case Renderer::Vulkan:
					break;
				default:
					throw std::invalid_argument("Renderer is not supported!");
			}
		}

		Window::~Window() {
			WindowCount--;
			if (WindowCount <= 0) {
				__Shutdown();
				IsInitialized = false;
			}
		}

		void Window::SetTitle(const std::string& title) {
			myTitle = title;
			glfwSetWindowTitle(myWindow, myTitle.c_str());
		}

		void Window::MakeCurrent() {
			glfwMakeContextCurrent(myWindow);
		}

		void Window::Poll() {
			glfwPollEvents();

			// We will poll all of the keys, and update their state. It's not optimal, but it works!
			for(uint32_t ix = GLFW_KEY_SPACE; ix <= GLFW_KEY_LAST; ix++) {
				const int pressed = glfwGetKey(myWindow, ix);
				// If the button was down last frame, we get a 1 in the second bit, otherwise it's a zero
				const ButtonState newState = (ButtonState)(((*myKeyStates[ix] & 0b01) << 1) | pressed);
				
				if (newState != myKeyStates[ix] && OnKeyChange != nullptr) {
					OnKeyChange(*this, static_cast<Key>(ix), newState, myUserPointer);
				}
				myKeyStates[ix] = newState;
			}

			// We will poll all of the mouse buttons, and update their state. It's not optimal, but it works!
			for (uint32_t ix = 0; ix <= GLFW_MOUSE_BUTTON_LAST; ix++) {
				const bool pressed = glfwGetMouseButton(myWindow, ix);
				// If the button was down last frame, we get a 1 in the second bit, otherwise it's a zero
				const ButtonState newState = (ButtonState)(((*myMouseStates[ix] & 0b01) << 1) | pressed);
				
				if (newState != myMouseStates[ix] && OnMouseButton != nullptr) {
					OnMouseButton(*this, static_cast<MouseButton>(ix), newState, myUserPointer);
				}
				myMouseStates[ix] = newState;
			}
		}

		void Window::SwapBuffers() {
			glfwSwapBuffers(myWindow);
		}


		bool Window::GetShouldClose() const {
			return glfwWindowShouldClose(myWindow);
		}

		void Window::Close() {
			glfwSetWindowShouldClose(myWindow, true);
		}

		void Window::Resize(uint32_t newWidth, uint32_t newHeight) {
			LOG_ASSERT(newWidth > 0, "Width must be greater than 0!");
			LOG_ASSERT(newHeight > 0, "Height must be greater than 0!");
			glfwSetWindowSize(myWindow, newWidth, newHeight);
		}

		glm::vec2 Window::GetCursorPos() const {
			double x, y;
			glfwGetCursorPos(myWindow, &x, &y);
			return { static_cast<float>(x), static_cast<float>(y) };
		}

		void Window::SetCursorPos(const glm::vec2& pos) {
			glfwSetCursorPos(myWindow, pos.x, pos.y);
		}

		ButtonState Window::GetKeyState(Key key) const {
			return myKeyStates[*key];
		}

		ButtonState Window::GetMouseButtonState(MouseButton button) const {
			return myMouseStates[*button];
		}

		void Window::SetSwapInterval(int interval)
		{
			GLFWwindow* ctx = glfwGetCurrentContext();
			glfwMakeContextCurrent(myWindow);
			glfwSwapInterval(interval);
			if (ctx != myWindow)
				glfwMakeContextCurrent(ctx);
		}

		void Window::__Initialize() {
			if (glfwInit() == GLFW_FALSE) {
				std::cout << "Failed to initialize GLFW" << std::endl;
				throw std::runtime_error("Failed to initialize GLFW");
			}

			IsInitialized = true;
		}

		void Window::__Shutdown() {
			glfwTerminate();
		}

		void Window::__InternalResizeEvent(uint32_t newWidth, uint32_t newHeight) {
			if (newWidth > 0 && newHeight > 0) {
				isMinimized = false;
				myWidth = newWidth;
				myHeight = newHeight;

				if (OnSizeChanged) {
					OnSizeChanged(*this, myWidth, myHeight, myUserPointer);
				}
			} else {
				isMinimized = true;
			}
		}
	}
}