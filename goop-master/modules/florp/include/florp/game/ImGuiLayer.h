#pragma once
#include "florp/app/ApplicationLayer.h"

namespace florp::game
{
	/*
	 * Provides IMGui functionality to the application
	 */
	class ImGuiLayer : public app::ApplicationLayer {
	public:
		void Initialize() override;
		void Shutdown() override;

		void PreGUI() override;
		void PostGUI() override;
	};
}
