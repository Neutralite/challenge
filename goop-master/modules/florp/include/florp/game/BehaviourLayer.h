#pragma once
#include "florp/app/ApplicationLayer.h"

namespace florp {
	namespace game {

		class BehaviourLayer : public app::ApplicationLayer {
		public:
			void Update() override;
			void LateUpdate() override;
			void FixedUpdate() override;
			void RenderGUI() override;
			void OnSceneEnter() override;
			void OnSceneExit() override;
		};
		
	}
}
