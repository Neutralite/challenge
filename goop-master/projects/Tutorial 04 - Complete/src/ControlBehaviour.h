#pragma once
#include "florp/game/IBehaviour.h"
#include "Logging.h"
#include "florp/game/Transform.h"
#include "florp/game/SceneManager.h"
#include "florp/app/Timing.h"
#include "imgui.h"

class ControlBehaviour : public florp::game::IBehaviour {
public:
	ControlBehaviour(const glm::vec3& speed) : IBehaviour(), mySpeed(speed) {};
	virtual ~ControlBehaviour() = default;

	virtual void Update(entt::entity entity) override {
		auto& transform = CurrentRegistry().get<florp::game::Transform>(entity);
		transform.Rotate(mySpeed * florp::app::Timing::DeltaTime);
	}

private:
	glm::vec3 mySpeed;
};


