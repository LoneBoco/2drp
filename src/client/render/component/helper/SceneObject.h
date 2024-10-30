#pragma once

#include "client/render/component/RenderComponent.h"
#include "client/render/component/TileMapRenderComponent.h"
#include "client/render/component/TMXRenderComponent.h"
#include "client/render/component/AnimationRenderComponent.h"
#include "client/render/component/TextRenderComponent.h"

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
			so->AddComponent<render::component::RenderComponent>();
			break;
		case SceneObjectType::TILEMAP:
			so->AddComponent<render::component::TileMapRenderComponent>();
			break;
		case SceneObjectType::TMX:
			so->AddComponent<render::component::TMXRenderComponent>();
			break;
		case SceneObjectType::ANIMATED:
			so->AddComponent<render::component::AnimationRenderComponent>();
			break;
		case SceneObjectType::TEXT:
			so->AddComponent<render::component::TextRenderComponent>();
			break;
	}
}

} // end namespace tdrp::handlers
