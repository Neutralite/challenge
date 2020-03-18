#pragma once
#include "florp/app/ApplicationLayer.h"

class RenderLayer : public florp::app::ApplicationLayer
{
public:
	virtual void OnSceneEnter() override;

	virtual void PreRender() override;

	// Render will be where we actually perform our rendering
	virtual void Render() override;
};
