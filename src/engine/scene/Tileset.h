#pragma once

#include "engine/common.h"

namespace tdrp::scene
{

// Represents the global tile ID boundries for an instance of a tileset.
// This data should be saved separately from the tileset data as it can change per map.
struct TilesetGID
{
	// Global tile ID for the first tile in the set.  Subtract the tile ID from the GID to get the local tile ID in the tileset.
	uint32_t FirstGID = 0;

	// Global tile ID for the last tile in the set.
	uint32_t LastGID;
};

// Tileset details.
struct Tileset
{
	// File path to the tileset image.
	std::string File;

	// Dimensions of each tile in pixels.
	Vector2du TileDimensions{ 16, 16 };

	// Number of tiles in the tileset.
	uint32_t TileCount;

	// Number of columns in a chunk of tiles.
	uint16_t Columns;

	// Number of rows in a chunk of tiles.
	uint16_t Rows;

	// Number of pixels surrounding the tileset.
	uint8_t Margin{ 0 };

	// Number of pixels between each tile.
	uint8_t Spacing{ 0 };

	/// <summary>
	/// Gets the X/Y position of the tile in the tileset.
	/// </summary>
	/// <param name="tile">The local tile ID.</param>
	/// <returns>The tile's X/Y position in whole tile units.</returns>
	Vector2du GetTilePosition(uint32_t tile) const noexcept
	{
		auto tiles_per_chunk = Columns * Rows;
		auto chunk = tile / tiles_per_chunk;
		return { (tile % Columns) + (chunk * Columns), (tile / Columns) % Rows };
	}

	/// <summary>
	/// Gets the pixel position of the tile in the tileset.
	/// </summary>
	/// <param name="tile">The local tile ID.</param>
	/// <returns>The tile's X/Y position in pixels.</returns>
	Vector2du GetTilePixelPosition(uint32_t tile) const noexcept
	{
		auto pos = GetTilePosition(tile);
		return { Margin + pos.x * (TileDimensions.x + Spacing), Margin + pos.y * (TileDimensions.y + Spacing) };
	}
};

using TilesetPtr = std::shared_ptr<Tileset>;

} // end namespace tdrp::scene
