#pragma once

namespace florp {
	namespace app {

		class Timing
		{
		public:
			// The target time step between update frames
			static float FixedTimeStep;
			// The target time step between frames/updates
			static float FrameTimeStep;
			// The time since the last frame in ms
			static float DeltaTime;
			// The time since the game has started
			static float GameTime;
			// The time since the scene has loaded
			static float TimeSinceLoad;
			// The time of the last frame
			static float LastFrame;
		};
	}
}