#pragma once

#include <chrono>

#include "engine/scene/SceneObject.h"
#include "engine/network/construct/Attributes.h"
#include "engine/network/packets/SceneObjectNew.pb.h"

namespace tdrp::network
{

inline packet::SceneObjectNew constructSceneObjectPacket(std::shared_ptr<SceneObject> sceneobject)
{
	packet::SceneObjectNew object;
	object.set_id(sceneobject->ID);
	object.set_type(static_cast<google::protobuf::uint32>(sceneobject->GetType()));
	object.set_class_(sceneobject->GetClass()->GetName());
	object.set_non_replicated(sceneobject->NonReplicated);
	object.set_attached_to(sceneobject->GetAttachedId());

	if (auto scenep = sceneobject->GetCurrentScene().lock())
	{
		object.set_scene(scenep->GetName());
	}

	//object.set_script()

	assignAllAttributesToPacket(object, sceneobject->Attributes, &packet::SceneObjectNew::add_attributes);
	assignAllAttributesToPacket(object, sceneobject->Properties, &packet::SceneObjectNew::add_properties);

	return object;
}

} // end namespace tdrp::network
