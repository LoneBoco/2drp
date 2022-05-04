#pragma once

#include "engine/scene/ObjectClass.h"
#include "engine/network/packets/ClassAdd.pb.h"

namespace tdrp::network
{

inline packet::ClassAdd constructClassAddPacket(std::shared_ptr<ObjectClass> objectclass)
{
	auto add_attribute = [](packet::ClassAdd_Attribute* element, const std::pair<uint16_t, std::shared_ptr<Attribute>>& attribute)
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

	packet::ClassAdd object;
	object.set_name(objectclass->GetName());
	object.set_scriptclient(objectclass->ScriptClient);
	// object.set_clientserver(objectclass->ScriptServer);

	for (const auto& attr : objectclass->Attributes)
	{
		auto attribute = object.add_attributes();
		add_attribute(attribute, attr);
	}

	return object;
}

} // end namespace tdrp::network
