#pragma once

#include "client/component/render/StaticRenderComponent.h"
#include "client/component/render/TileMapRenderComponent.h"
#include "client/component/render/TMXRenderComponent.h"
#include "client/component/render/AnimationRenderComponent.h"
#include "client/component/render/TextRenderComponent.h"

#include "engine/common.h"
#include "engine/scene/SceneObject.h"

namespace tdrp::handlers
{

static void AddRenderComponent(SceneObjectPtr so)
{
	switch (so->GetType())
	{
		default:
		case SceneObjectType::STATIC:
			so->AddComponent<component::render::StaticRenderComponent>();
			break;
		case SceneObjectType::TILEMAP:
			so->AddComponent<component::render::TileMapRenderComponent>();
			break;
		case SceneObjectType::TMX:
			so->AddComponent<component::render::TMXRenderComponent>();
			break;
		case SceneObjectType::ANIMATED:
			so->AddComponent<component::render::AnimationRenderComponent>();
			break;
		case SceneObjectType::TEXT:
			so->AddComponent<component::render::TextRenderComponent>();
			break;
	}
}

} // end namespace tdrp::handlers
