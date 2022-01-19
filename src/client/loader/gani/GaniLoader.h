#pragma once

#include <set>

#include "engine/common.h"
#include "engine/filesystem/common.h"

namespace tdrp::loader
{

struct SSprite
{
	SSprite() {}
	SSprite(const std::string& image, const Recti& source) : Image(image), Source(source) {}

	std::string Image;
	Recti Source;
};

struct SFrame
{
	struct SAniSound
	{
		std::string Sound;
		float X;
		float Y;
	};

	struct SAniSprite
	{
		uint32_t Index;
		int X;
		int Y;
	};

	SFrame(int delay) : Delay(delay) {}

	uint32_t Delay;
	std::vector<SAniSound> Sounds;
	std::vector<std::vector<SAniSprite>> Sprites;
};

class GaniLoader;
class GaniAnimation
{
	friend class tdrp::loader::GaniLoader;

public:
	GaniAnimation() = default;
	~GaniAnimation() = default;

	GaniAnimation(const GaniAnimation& other) = delete;
	GaniAnimation(GaniAnimation&& other) = delete;
	GaniAnimation& operator=(const GaniAnimation& other) = delete;
	GaniAnimation& operator=(GaniAnimation&& other) = delete;
	bool operator==(const GaniAnimation& other) = delete;

public:

public:
	bool IsLooped = false;
	bool IsContinuous = false;
	uint8_t Directions = 4;
	std::string Setbackto;

	std::map<uint32_t, SSprite> Sprites;
	std::vector<SFrame> Frames;
	std::set<std::string> Sounds;
};


class GaniLoader
{
public:
	GaniLoader() = delete;
	~GaniLoader() = delete;

	GaniLoader(const GaniLoader& other) = delete;
	GaniLoader(GaniLoader&& other) = delete;
	GaniLoader& operator=(const GaniLoader& other) = delete;
	GaniLoader& operator=(GaniLoader&& other) = delete;
	bool operator==(const GaniLoader& other) = delete;

public:
	static std::shared_ptr<GaniAnimation> LoadGani(const filesystem::path& file);

private:
};

} // end namespace tdrp::loader
