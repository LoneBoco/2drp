#pragma once

#include <chrono>

#include "engine/scene/SceneObject.h"
#include "engine/scene/ObjectAttributes.h"

namespace tdrp::network
{

template <class F>
struct return_type;

template <class R, class M, class... A>
struct return_type<R(M::*)(A...)>
{
	typedef R type;
};

template <class T>
using return_type_v = typename return_type<T>::type;


template <class T>
concept HasUpdateRate = requires (T a)
{
	a->set_update_rate(0);
};

///////////////////////////////////////////////////////////////////////////////

template <typename P, typename A>
P& processAndAssignAttributesToPacket(P& packet, A& attributes, auto adder, const std::chrono::milliseconds& elapsed)
{
	// Loop through all attributes and process them.
	for (auto& [id, attribute] : attributes)
	{
		bool networkQueued = attributes.NetworkQueuedAttributes.contains(id);

		// Process the attribute and return if its not dirty or was not queue for network update.
		if (!attribute->ProcessDirty() && !networkQueued)
			continue;

		// Update the network cooldown.
		if (networkQueued)
			attribute->UpdateCooldown(elapsed);

		// Send the attribute if we're ready for it.
		if (attribute->IsNetworkReadyForUpdate())
		{
			auto attr = std::invoke(adder, packet);
			attr->set_id(id);
			attr->set_name(attribute->Name);
			attr->set_replicated(attribute->Replicated);

			if constexpr (HasUpdateRate<return_type_v<decltype(adder)>>)
			{
				attr->set_update_rate(static_cast<google::protobuf::uint32>(attribute->NetworkUpdateRate.count()));
			}

			switch (attribute->GetType())
			{
				case AttributeType::INTEGER:
					attr->set_as_int(attribute->GetAs<int64_t>());
					break;
				case AttributeType::DOUBLE:
					attr->set_as_double(attribute->GetAs<double>());
					break;
				default:
				case AttributeType::STRING:
					attr->set_as_string(attribute->GetAs<std::string>());
					break;
			}

			// Remove the attribute from our network queued list.
			if (networkQueued)
				attributes.NetworkQueuedAttributes.erase(id);

			// Set the attribute network cooldown now that we've sent it.
			attribute->SetUpdateCooldown();
		}
		// If we have a rate limit on this attribute, add it to the network queue to send later.
		else if (!networkQueued)
		{
			attributes.NetworkQueuedAttributes.insert(id);
		}
	}

	return packet;
}

template <typename P, typename A>
P& assignAllAttributesToPacket(P& packet, A& attributes, auto adder)
{
	// Loop through all attributes and add them to the packet.
	for (auto& [id, attribute] : attributes)
	{
		if (attribute->GetType() == AttributeType::INVALID)
			continue;

		auto element = std::invoke(adder, packet);
		element->set_id(id);
		element->set_name(attribute->Name);
		element->set_replicated(attribute->Replicated);

		if constexpr (HasUpdateRate<return_type_v<decltype(adder)>>)
		{
			element->set_update_rate(static_cast<google::protobuf::uint32>(attribute->NetworkUpdateRate.count()));
		}

		switch (attribute->GetType())
		{
		case AttributeType::INTEGER:
			element->set_as_int(attribute->GetAs<int64_t>());
			break;
		case AttributeType::DOUBLE:
			element->set_as_double(attribute->GetAs<double>());
			break;
		case AttributeType::STRING:
			element->set_as_string(attribute->GetAs<std::string>());
			break;
		default:
			element->set_as_string("Unknown Attribute Error");
			break;
		}
	}

	return packet;
}

} // end namespace tdrp::network
