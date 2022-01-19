#pragma once

#include <spriterengine/override/atlasfile.h>

#include <SFML/Graphics.hpp>

namespace tdrp::loader::spriter
{

class SfmlAtlasFile : public SpriterEngine::AtlasFile
{
public:
	SfmlAtlasFile(std::string initialFilePath);

	const sf::Texture *getTexture() const;

	bool loaded() {return m_loaded;}

private:
	std::weak_ptr<sf::Texture> texture;

	void initializeFile();

	void renderSprite(SpriterEngine::UniversalObjectInterface *spriteInfo, const SpriterEngine::atlasframedata data) override;

	bool m_loaded;
};

}
