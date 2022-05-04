#pragma once

#include "engine/scene/SceneObject.h"
#include "engine/network/packets/SceneObjectNew.pb.h"

namespace tdrp::network
{

inline packet::SceneObjectNew constructSceneObjectPacket(std::shared_ptr<SceneObject> sceneobject)
{
	auto add_attribute = [](packet::SceneObjectNew_Attribute* element, const std::pair<uint16_t, std::shared_ptr<Attribute>>& attribute)
	{
		if (attribute.second->GetType() == AttributeType::INVALID)
			return;

		element->set_id(attribute.first);
		element->set_name(attribute.second->GetName());

		switch (attribute.second->GetType())
		{
		case AttributeType::SIGNED:
			element->set_as_int(attribute.second->GetSigned());
			break;
		case AttributeType::UNSIGNED:
			element->set_as_uint(attribute.second->GetUnsigned());
			break;
		case AttributeType::FLOAT:
			element->set_as_float(attribute.second->GetFloat());
			break;
		case AttributeType::DOUBLE:
			element->set_as_double(attribute.second->GetDouble());
			break;
		case AttributeType::STRING:
			element->set_as_string(attribute.second->GetString());
			break;
		default:
			element->set_as_string("Unknown Attribute Error");
			break;
		}
	};

	packet::SceneObjectNew object;
	object.set_id(sceneobject->ID);
	object.set_type(static_cast<google::protobuf::uint32>(sceneobject->GetType()));
	object.set_class_(sceneobject->GetClass()->GetName());
	object.set_non_replicated(sceneobject->NonReplicated);

	if (auto scenep = sceneobject->GetCurrentScene().lock())
	{
		object.set_scene(scenep->GetName());
	}

	//object.set_script()

	for (const auto& prop : sceneobject->Properties)
	{
		auto attribute = object.add_properties();
		add_attribute(attribute, prop);
	}

	for (const auto& attr : sceneobject->Attributes)
	{
		auto attribute = object.add_attributes();
		add_attribute(attribute, attr);
	}

	return object;
}

} // end namespace tdrp::network
