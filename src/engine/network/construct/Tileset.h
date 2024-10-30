#pragma once

#include "engine/scene/Tileset.h"
#include "engine/network/packets/SceneTilesetAdd.pb.h"

namespace tdrp::network
{

inline packet::SceneTilesetAdd constructTilesetAddPacket(std::shared_ptr<scene::Tileset> tileset)
{
	packet::SceneTilesetAdd packet{};
	auto* ts = packet.add_tilesets();
	ts->set_name(tileset->File);
	ts->set_tilewidth(tileset->TileDimensions.x);
	ts->set_tileheight(tileset->TileDimensions.y);
	ts->set_tilecount(tileset->TileCount);
	ts->set_columns(tileset->Columns);
	ts->set_rows(tileset->Rows);
	ts->set_margin(tileset->Margin);
	ts->set_spacing(tileset->Spacing);
	return packet;
}

} // end namespace tdrp::network
