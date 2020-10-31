#pragma once

#include "engine/scene/SceneObject.h"
#include "engine/network/packets/SSceneObjectNew.pb.h"

namespace tdrp::network
{

inline packet::SSceneObjectNew constructSceneObjectPacket(std::shared_ptr<SceneObject> sceneobject)
{
	auto add_attribute = [](packet::SSceneObjectNew& packet, const std::pair<uint16_t, std::shared_ptr<Attribute>>& attribute)
	{
		if (attribute.second->GetType() == AttributeType::INVALID)
			return;

		auto n = packet.add_properties();
		n->set_id(attribute.first);
		n->set_name(attribute.second->GetName());

		switch (attribute.second->GetType())
		{
		case AttributeType::SIGNED:
			n->set_as_int(attribute.second->GetSigned());
			break;
		case AttributeType::UNSIGNED:
			n->set_as_uint(attribute.second->GetUnsigned());
			break;
		case AttributeType::FLOAT:
			n->set_as_float(attribute.second->GetFloat());
			break;
		case AttributeType::DOUBLE:
			n->set_as_double(attribute.second->GetDouble());
			break;
		case AttributeType::STRING:
			n->set_as_string(attribute.second->GetString());
			break;
		default:
			n->set_as_string("Unknown Attribute Error");
			break;
		}
	};

	packet::SSceneObjectNew object;
	object.set_id(sceneobject->ID);
	object.set_type(static_cast<google::protobuf::uint32>(sceneobject->GetType()));
	object.set_class_(sceneobject->GetClass()->GetName());
	//object.set_script()

	for (const auto& prop : sceneobject->Properties)
	{
		add_attribute(object, prop);
	}

	for (const auto& attr : sceneobject->Attributes)
	{
		add_attribute(object, attr);
	}

	return object;
}

} // end namespace tdrp::network
