#include "florp/app/Timing.h"

namespace florp {
	namespace  app {

		float Timing::FixedTimeStep = 1.0f / 30.0f;
		float Timing::FrameTimeStep = 1.0f / 60.0f;
		float Timing::DeltaTime = 0.0f;
		float Timing::GameTime = 0.0f;
		float Timing::TimeSinceLoad = 0.0f;
		float Timing::LastFrame = 0.0f;
		
	}
}