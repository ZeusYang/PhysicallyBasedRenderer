#pragma once

#include "Renderer/RenderSystem.h"

class Scene
{
public:
	typedef std::shared_ptr<Scene> ptr;

	Scene() = default;
	virtual ~Scene() = default;

	virtual void initializeScene(Renderer::RenderSystem::ptr renderSys) = 0;

};

class SponzaScene : public Scene
{
public:
	typedef std::shared_ptr<SponzaScene> ptr;

	SponzaScene() = default;
	virtual ~SponzaScene() = default;

	virtual void initializeScene(Renderer::RenderSystem::ptr renderSys);
};

class FelLordScene : public Scene
{
public:
	typedef std::shared_ptr<FelLordScene> ptr;

	FelLordScene() = default;
	virtual ~FelLordScene() = default;

	virtual void initializeScene(Renderer::RenderSystem::ptr renderSys);
};