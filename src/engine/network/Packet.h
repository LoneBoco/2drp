#pragma once

#include <cstdint>

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
	T packet{};
	packet.ParseFromArray(packet_data, static_cast<int>(packet_length));
	return packet;
}

//! If this changes, it means we re-ordered the packet numbers or did something else to make versions incompatible.
inline constexpr int PACKETVERSION = 1;

// Don't adjust packet order without changing PACKETVERSION.
enum class Packets
{
	NOTHING = 0,
	ERROR,
	CLIENTREADY,
	LOGIN,
	LOGINSTATUS,
	SERVERINFO,
	FILEREQUEST,
	FILETRANSFER,
	SWITCHSCENE,
	CLIENTSCRIPTADD,
	CLIENTSCRIPTDELETE,
	CLASSADD,
	CLASSDELETE,
	SCENEOBJECTNEW,
	SCENEOBJECTCHANGE,
	SCENEOBJECTDELETE,
	SCENEOBJECTOWNERSHIP,
	SCENEOBJECTCONTROL,
	SCENEOBJECTUNFOLLOW,
	SCENEOBJECTSHAPES,
	SCENEOBJECTCHUNKDATA,
	SCENEOBJECTREQUESTCHUNKDATA,
	SCENETILESETADD,
	SENDEVENT,
	FLAGSET,
	ITEMDEFINITION,
	ITEMADD,
	ITEMCOUNT,

	COUNT
};

} // end namespace tdrp::network
