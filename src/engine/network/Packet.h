#pragma once

#undef ERROR

namespace tdrp::network
{

template <typename T>
constexpr uint16_t PACKETID(T packet)
{
	return static_cast<uint16_t>(packet);
};

template <class T>
const T construct(const uint8_t* const packet_data, const size_t packet_length)
{
	T packet;
	packet.ParseFromArray(packet_data, packet_length);
	return packet;
}

//! If this changes, it means we re-ordered the packet numbers or did something else to make versions incompatible.
constexpr int PACKETVERSION = 1;

// Don't adjust packet order without changing PACKETVERSION.
enum class ClientPackets
{
	NOTHING = 0,
	LOGIN,
	REQUESTFILE,
	SCENEOBJECTCHANGE,
	SENDEVENT,

	SERVERATTRIBUTES,

	COUNT
};

// Don't adjust packet order without changing PACKETVERSION.
enum class ServerPackets
{
	NOTHING = 0,
	ERROR,
	LOGINSTATUS,
	PACKAGEFILES,
	TRANSFERFILE,
	CLIENTSCRIPT,
	CLIENTSCRIPTDELETE,
	CLASS,
	CLASSDELETE,
	SCENEOBJECTNEW,
	SCENEOBJECTCHANGE,
	SCENEOBJECTDELETE,
	SENDEVENT,

	SERVERTYPE,				// [type]
	CLASSADD,				// [name]
	CLASSATTRIBUTEADD,		// [INT1 class length][class][INT2 id][name]
	CLASSSCRIPT,			// [STRING class name][script]
	SERVERATTRIBUTEADD,		// [INT2 id][name]
	SERVERATTRIBUTES,		// {[INT2 id][value], ...}
	OBJECTADD,				// [INT4 id][INT1 type][class]
	OBJECTDELETE,			// [INT4 id]
	OBJECTATTRIBUTES,		// [INT4 id] {[INT2 id][value], ...}
	OBJECTPROPERTIES,		// [INT4 id] {[INT1 id][value], ...}
	OBJECTSCRIPT,			// [INT4 id][script]
	PLAYEROBJECT,			// [INT4 id]
	PHYSICS_ADD_POLYGON,	// [INT4 id] ([INT1 0][FLOAT width][FLOAT height] / [INT1 1]{[FLOAT x1][FLOAT y1], ...})

	COUNT
};

} // end namespace tdrp::network
