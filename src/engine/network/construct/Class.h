#pragma once

#include "engine/scene/ObjectClass.h"
#include "engine/network/construct/Attributes.h"
#include "engine/network/packets/ClassAdd.pb.h"

namespace tdrp::network
{

inline packet::ClassAdd constructClassAddPacket(std::shared_ptr<ObjectClass> objectclass)
{
	packet::ClassAdd object;
	object.set_name(objectclass->GetName());
	object.set_scriptclient(objectclass->ScriptClient);
	// object.set_clientserver(objectclass->ScriptServer);

	assignAllAttributesToPacket(object, objectclass->Attributes, &packet::ClassAdd::add_attributes);

	return object;
}

} // end namespace tdrp::network
