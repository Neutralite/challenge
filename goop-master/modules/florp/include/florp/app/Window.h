#pragma once
#include <cstdint>
#include <EnumToString.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace florp {
	namespace app {

		ENUM(Renderer, uint32_t,
			OpenGL,
			Vulkan
		);

		ENUM(ButtonState, uint8_t,
			Up = 0b00,
			Down = 0b11,
			Pressed = 0b01,
			Released = 0b10
		);

		ENUM(Key, uint32_t,
			Space = GLFW_KEY_SPACE,
			Apostrophe = GLFW_KEY_APOSTROPHE, /* ' */
			Comma = GLFW_KEY_COMMA, /* , */
			Minus = GLFW_KEY_MINUS, /* - */
			Period = GLFW_KEY_PERIOD, /* . */
			Slash = GLFW_KEY_SLASH, /* / */
			Zero = GLFW_KEY_0,
			One = GLFW_KEY_1,
			Two = GLFW_KEY_2,
			Three = GLFW_KEY_3,
			Four = GLFW_KEY_4,
			Five = GLFW_KEY_5,
			Six = GLFW_KEY_6,
			Seven = GLFW_KEY_7,
			Eight = GLFW_KEY_8,
			Nine = GLFW_KEY_9,
			Semicolon = GLFW_KEY_SEMICOLON, /* ; */
			Equal = GLFW_KEY_EQUAL, /* = */
			A = GLFW_KEY_A,
			B = GLFW_KEY_B,
			C = GLFW_KEY_C,
			D = GLFW_KEY_D,
			E = GLFW_KEY_E,
			F = GLFW_KEY_F,
			G = GLFW_KEY_G,
			H = GLFW_KEY_H,
			I = GLFW_KEY_I,
			J = GLFW_KEY_J,
			K = GLFW_KEY_K,
			L = GLFW_KEY_L,
			M = GLFW_KEY_M,
			N = GLFW_KEY_N,
			O = GLFW_KEY_O,
			P = GLFW_KEY_P,
			Q = GLFW_KEY_Q,
			R = GLFW_KEY_R,
			S = GLFW_KEY_S,
			T = GLFW_KEY_T,
			U = GLFW_KEY_U,
			V = GLFW_KEY_V,
			W = GLFW_KEY_W,
			X = GLFW_KEY_X,
			Y = GLFW_KEY_Y,
			Z = GLFW_KEY_Z,
			LeftBracket = GLFW_KEY_LEFT_BRACKET, /* [ */
			Backslash = GLFW_KEY_BACKSLASH, /* \ */
			RightBracket = GLFW_KEY_RIGHT_BRACKET, /* ] */
			Grave = GLFW_KEY_GRAVE_ACCENT, /* ` */
			World1 = GLFW_KEY_WORLD_1, /* non-US #1 */
			World2 = GLFW_KEY_WORLD_2, /* non-US #2 */

			Esc = GLFW_KEY_ESCAPE,
			Enter = GLFW_KEY_ENTER,
			Tab = GLFW_KEY_TAB,
			Backspace = GLFW_KEY_BACKSPACE,
			Insert = GLFW_KEY_INSERT,
			Delete = GLFW_KEY_DELETE,
			Right = GLFW_KEY_RIGHT,
			Left = GLFW_KEY_LEFT,
			Down = GLFW_KEY_DOWN,
			Up = GLFW_KEY_UP,
			PageUp = GLFW_KEY_PAGE_UP,
			PageDown = GLFW_KEY_PAGE_DOWN,
			Home = GLFW_KEY_HOME,
			End = GLFW_KEY_END,
			CapsLock = GLFW_KEY_CAPS_LOCK,
			ScrollLock = GLFW_KEY_SCROLL_LOCK,
			NumLock = GLFW_KEY_NUM_LOCK,
			PrintScreen = GLFW_KEY_PRINT_SCREEN,
			Pause = GLFW_KEY_PAUSE,
			F1 = GLFW_KEY_F1,
			F2 = GLFW_KEY_F2,
			F3 = GLFW_KEY_F3,
			F4 = GLFW_KEY_F4,
			F5 = GLFW_KEY_F5,
			F6 = GLFW_KEY_F6,
			F7 = GLFW_KEY_F7,
			F8 = GLFW_KEY_F8,
			F9 = GLFW_KEY_F9,
			F10 = GLFW_KEY_F10,
			F11 = GLFW_KEY_F11,
			F12 = GLFW_KEY_F12,
			F13 = GLFW_KEY_F13,
			F14 = GLFW_KEY_F14,
			F15 = GLFW_KEY_F15,
			F16 = GLFW_KEY_F16,
			F17 = GLFW_KEY_F17,
			F18 = GLFW_KEY_F18,
			F19 = GLFW_KEY_F19,
			F20 = GLFW_KEY_F20,
			F21 = GLFW_KEY_F21,
			F22 = GLFW_KEY_F22,
			F23 = GLFW_KEY_F23,
			F24 = GLFW_KEY_F24,
			F25 = GLFW_KEY_F25,
			Keypad0 = GLFW_KEY_KP_0,
			Keypad1 = GLFW_KEY_KP_1,
			Keypad2 = GLFW_KEY_KP_2,
			Keypad3 = GLFW_KEY_KP_3,
			Keypad4 = GLFW_KEY_KP_4,
			Keypad5 = GLFW_KEY_KP_5,
			Keypad6 = GLFW_KEY_KP_6,
			Keypad7 = GLFW_KEY_KP_7,
			Keypad8 = GLFW_KEY_KP_8,
			Keypad9 = GLFW_KEY_KP_9,
			KeypadDecimal = GLFW_KEY_KP_DECIMAL,
			KeypadDivide = GLFW_KEY_KP_DIVIDE,
			KeypadMultiply = GLFW_KEY_KP_MULTIPLY,
			KeypadSubract = GLFW_KEY_KP_SUBTRACT,
			KeypadAdd = GLFW_KEY_KP_ADD,
			KeypadEnter = GLFW_KEY_KP_ENTER,
			KeypadEqual = GLFW_KEY_KP_EQUAL,
			LeftShift = GLFW_KEY_LEFT_SHIFT,
			LeftControl = GLFW_KEY_LEFT_CONTROL,
			LeftAlt = GLFW_KEY_LEFT_ALT,
			LeftSuper = GLFW_KEY_LEFT_SUPER,
			RightShift = GLFW_KEY_RIGHT_SHIFT,
			RightControl = GLFW_KEY_RIGHT_CONTROL,
			RightAlt = GLFW_KEY_RIGHT_ALT,
			RightSuper = GLFW_KEY_RIGHT_SUPER,
			Menu = GLFW_KEY_MENU
		);

		ENUM(MouseButton, uint32_t,
			Left = GLFW_MOUSE_BUTTON_1,
			Right = GLFW_MOUSE_BUTTON_2,
			Middle = GLFW_MOUSE_BUTTON_3,
			Four = GLFW_MOUSE_BUTTON_4,
			Five = GLFW_MOUSE_BUTTON_5,
			Six = GLFW_MOUSE_BUTTON_6,
			Seven = GLFW_MOUSE_BUTTON_7,
			Eight = GLFW_MOUSE_BUTTON_8
		);

		class Window;

		typedef void(*WindowSizeCallback)(Window& window, uint32_t width, uint32_t height, void* userPointer);
		typedef void(*WindowKeyEventCallback)(Window& window, Key key, ButtonState state, void* userPointer);
		typedef void(*WindowMouseButtonEventCallback)(Window& window, MouseButton button, ButtonState state, void* userPointer);

		class Window {
		public:
			typedef std::shared_ptr<Window> Sptr;

			// Invoked when the size of the window has changed
			WindowSizeCallback             OnSizeChanged;
			// Invoked when a key state has changed, invoked from main thread during polling
			WindowKeyEventCallback         OnKeyChange;
			// Invoked when a mouse button state has changed, invoked from main thread during polling
			WindowMouseButtonEventCallback OnMouseButton;

			/*
			 * Creates a new window, initializing GLFW and GLAD where required
			 * @param width The width of the window in pixels (default 800)
			 * @param height The height of the window in pixels (default 600)
			 * @param title The text to show in the window title bar, task bar, task manager, etc...
			 * @param renderer The rendering subsystem to use, by default this is OpenGL (Vulkan will be implemented later)
			 */
			Window(uint32_t width = 800u, uint32_t height = 600u, const std::string& title = "Hello World", Renderer renderer = Renderer::OpenGL);
			virtual ~Window();

			uint32_t GetWidth() const { return myWidth; }
			uint32_t GetHeight() const { return myHeight; }

			/*
			 * Gets the GLFW window handle associated with this window
			 */
			GLFWwindow* GetHandle() const { return myWindow; }

			void SetTitle(const std::string& title);
			const std::string& GetTitle() const { return myTitle; }

			// Makes this window the current window for render events
			void MakeCurrent();

			/*
			 *	Polls this window for events such as move, resize, input, etc, and calculates
			 *	the state of the inputs
			 */
			void Poll();
			/*
			 * Swaps this window's front and back buffers
			 */
			void SwapBuffers();

			/*
			 * Gets whether or not the window should close on the next iteration of the game loop
			 */
			bool GetShouldClose() const;
			/*
			 * Tells this window to close on the next iteration of the game loop
			 */
			void Close();

			/*
			 * Sets a user pointer for this window that can be passed to callbacks. Note
			 * that a window can only have one user pointer, so this will override any
			 * pointer that is currently set, potentially resulting in a memory leak if
			 * your pointer is not tracked elsewhere!
			 * @param value The pointer to set as the window's user pointer
			 */
			void SetUserPointer(void* value) { myUserPointer = value; }
			/*
			 * Gets this windows user pointer
			 */
			void* GetUserPointer() const { return myUserPointer; }

			/*
			 * Resizes this window to the new dimensions. Note that width and height must both be greater than 0
			 * @param newWidth The new width of the window, in pixels
			 * @param newHeight The new height of the window, in pixels
			 */
			void Resize(uint32_t newWidth, uint32_t newHeight);

			/*
			 * Gets the cursor's position relative to the upper-left corner of this screen. Note that for OpenGL,
			 * this means that the y value will be inverted (ie, to get a y coordinate in the OpenGL coordinate system,
			 * you should take cursorPos.y - Window.GetHeight())
			 * @returns The cursor's position relative to the top-left corner of this window
			 */
			glm::vec2 GetCursorPos() const;
			/*
			 * Sets the cursor's position relative to the upper-left corner of this screen. Note that for OpenGL,
			 * this means that the y value will be inverted (ie, to get a y coordinate in the OpenGL coordinate system,
			 * you should take cursorPos.y - Window.GetHeight())
			 * @param pos The new position for the cursor, relative to the top-left of the screen
			 */
			void SetCursorPos(const glm::vec2& pos);

			/*
			 * Gets the current state of a keyboard key since the last Poll
			 * @param key The key who's state to check
			 * @returns The ButtonState of that key (up, pressed, down, released)
			 */
			ButtonState GetKeyState(Key key) const;
			/*
			 * Gets whether the given key is currently pressed or down
			 * @param key The key to check
			 * @returns True if the keyState is Pressed or Down, false if otherwise
			 */
			bool IsKeyDown(Key key) const { return *GetKeyState(key) & 0x01; }

			/*
			 * Gets the current state of a mouse button since the last Poll
			 * @param button The mouse button who's state to check
			 * @returns The ButtonState of that button (up, pressed, down, released)
			 */
			ButtonState GetMouseButtonState(MouseButton button) const;
			/*
			 * Gets whether the given mouse button is currently pressed or down
			 * @param button The button to check
			 * @returns True if the button state is Pressed or Down, false if otherwise
			 */
			bool IsMouseButtonDown(MouseButton button) const { return *GetMouseButtonState(button) & 0x01; }
			/*
			 * Sets the number of screen updates to wait before swapping your buffers
			 * @param interval The minimum number of screen updates to wait for until the buffers are swapped
			 */
			void SetSwapInterval(int interval);

		protected:
			uint32_t    myWidth, myHeight;
			std::string myTitle;
			GLFWwindow* myWindow;
			void* myUserPointer;
			ButtonState myKeyStates[GLFW_KEY_LAST + 1];
			ButtonState myMouseStates[GLFW_MOUSE_BUTTON_LAST + 1];
			bool        isMinimized;

			void __Initialize();
			void __Shutdown();

			void __InternalResizeEvent(uint32_t newWidth, uint32_t newHeight);

			static void ResizeProxy(GLFWwindow* window, int width, int height);

			static uint32_t WindowCount;
			static bool IsInitialized;
		};

	}
}
