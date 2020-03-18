#include "florp/game/BehaviourLayer.h"
#include "florp/game/SceneManager.h"
#include "florp/game/IBehaviour.h"

namespace florp::game {
	
	void BehaviourLayer::Update() {
		auto view = CurrentRegistry().view<BehaviourBinding>();
		if (view.size() > 0) {
			view.each([](auto entity, BehaviourBinding& binding) {
				for (std::shared_ptr<IBehaviour> b : binding.Behaviours) {
					if (b->Enabled) b->Update(entity);
				}
			});
		}
	}

	void BehaviourLayer::LateUpdate() {
		auto view = CurrentRegistry().view<BehaviourBinding>();
		if (view.size() > 0) {
			view.each([](auto entity, BehaviourBinding& binding) {
				for (std::shared_ptr<IBehaviour> b : binding.Behaviours) {
					if (b->Enabled) b->LateUpdate(entity);
				}
			});
		}
	}

	void BehaviourLayer::FixedUpdate()
	{
		auto view = CurrentRegistry().view<BehaviourBinding>();
		if (view.size() > 0) {
			view.each([](auto entity, BehaviourBinding& binding) {
				for (std::shared_ptr<IBehaviour> b : binding.Behaviours) {
					if (b->Enabled) b->FixedUpdate(entity);
				}
			});
		}
	}

	void BehaviourLayer::RenderGUI() {
		auto view = CurrentRegistry().view<BehaviourBinding>();
		if (view.size() > 0) {
			view.each([](auto entity, BehaviourBinding& binding) {
				for (std::shared_ptr<IBehaviour> b : binding.Behaviours) {
					if (b->Enabled) b->RenderGUI(entity);
				}
			});
		}
	}

	void BehaviourLayer::OnSceneEnter() {
		auto view = CurrentRegistry().view<BehaviourBinding>();
		if (view.size() > 0) {
			view.each([](auto entity, BehaviourBinding& binding) {
				for (std::shared_ptr<IBehaviour> b : binding.Behaviours) {
					if (b->Enabled) b->OnLoad(entity);
				}
			});
		}
	}

	void BehaviourLayer::OnSceneExit() {
		auto view = CurrentRegistry().view<BehaviourBinding>();
		if (view.size() > 0) {
			view.each([](auto entity, BehaviourBinding& binding) {
				for (std::shared_ptr<IBehaviour> b : binding.Behaviours) {
					if (b->Enabled) b->OnUnload(entity);
				}
			});
		}
	}
}
