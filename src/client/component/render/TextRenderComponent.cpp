#include <math.h>

#include "client/game/Game.h"
#include "client/component/render/TextRenderComponent.h"
#include "client/loader/SFMListream.h"
#include "client/loader/ResourceLoaders.h"

#include "engine/filesystem/File.h"
#include "engine/scene/SceneObject.h"
#include "engine/resources/Resource.h"


namespace tdrp::component::render
{

void TextRenderComponent::Initialize(ComponentEntity& owner)
{
	if (auto p_so = dynamic_cast<SceneObject*>(&owner))
	{
		// Re-acquire our scene object from the server so it is properly reference counted by shared_ptr.
		auto game = BabyDI::Get<tdrp::Game>();
		m_owner = game->Server->GetSceneObjectById(p_so->ID);

		// Bind our provider.
		owner.RegisterProvider("Size", std::bind(&TextRenderComponent::provide_size, this));
		owner.RegisterProvider("BoundingBox", std::bind(&TextRenderComponent::provide_boundingbox, this));
	}
}

void TextRenderComponent::OnAttached(ComponentEntity& owner)
{
	auto resources = BabyDI::Get<tdrp::ResourceManager>();

	if (auto so = m_owner.lock())
	{
		if (so->GetType() == SceneObjectType::TEXT)
		{
			auto text_so = std::dynamic_pointer_cast<TextSceneObject>(so);
			const auto& font = text_so->GetFont();
			if (!font.empty())
				load_font(font);
		}

		m_handle_text_change = so->Properties.Get(Property::TEXT)->UpdateDispatch.Subscribe(std::bind(&TextRenderComponent::property_update, this, std::placeholders::_1));
	}
}

void TextRenderComponent::OnDetached(ComponentEntity& owner)
{

}

/////////////////////////////

Rectf TextRenderComponent::GetBoundingBox() const
{
	if (!m_text.has_value())
		return Rectf{};

	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::TEXT)
			return Rectf{};

		const auto& text = m_text.value();

		auto pos = so->GetPosition();
		auto textpos = text.getPosition();
		auto bounds = text.getLocalBounds();
		auto global = text.getGlobalBounds();

		if (Vector2df::Distance(pos, { textpos.x, textpos.y }) > 100)
			return Rectf{ pos.x + bounds.position.x, pos.y + bounds.position.y, bounds.size.x, bounds.size.y };
		else return Rectf{ global.position.x, global.position.y, global.size.x, global.size.y };
	}

	return Rectf{};
}

void TextRenderComponent::Render(sf::RenderTarget& window, const Rectf& viewRect, std::chrono::milliseconds elapsed)
{
	if (!m_text.has_value() || m_text.value().getString().isEmpty())
		return;
	if (!m_good_font)
		return;

	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::TEXT)
			return;

		auto text_so = std::dynamic_pointer_cast<TextSceneObject>(so);
		if (!text_so)
			return;

		auto pos = so->GetPosition();
		auto scale = so->GetScale();
		auto& text = m_text.value();

		text.setPosition({ pos.x, pos.y });
		text.setScale({ scale.x, scale.y });
		text.setStyle(sf::Text::Bold);
		text.setFillColor(sf::Color::White);
		text.setOutlineColor(sf::Color::Black);
		text.setOutlineThickness(1.0f);

		if (text_so->GetCentered())
		{
			auto global = text.getGlobalBounds();
			auto local = text.getLocalBounds();
			auto origin = local.getCenter();
			origin.x = std::roundf(origin.x);
			origin.y = std::roundf(origin.y);
			text.setOrigin(origin);
		}
		else
		{
			text.setOrigin({ 0.f, 0.f });
		}

		// Draw the bounding box first to avoid a weird SFML bug.
		if (Settings->GetAs<bool>("Debug.drawtextbbox"))
		{
			sf::RectangleShape shape;
			auto bbox = GetBoundingBox();
			shape.setFillColor(sf::Color::Transparent);
			shape.setOutlineColor(sf::Color::Red);
			shape.setOutlineThickness(1.0f);
			shape.setPosition({ bbox.pos.x, bbox.pos.y });
			shape.setSize({ bbox.size.x, bbox.size.y });
			window.draw(shape);
		}

		window.draw(text);
	}
}

///////////////////////////////////////////////////////////////////////////////

void TextRenderComponent::load_font(const std::string& name)
{
	if (name == m_font_name)
		return;

	m_font_name = name;
	m_good_font = false;

	auto resources = BabyDI::Get<tdrp::ResourceManager>();
	auto id = resources->FindId<sf::Font>(name);
	if (id != 0)
	{
		auto handle = resources->Get<sf::Font>(id).lock();
		m_font = handle;
		m_text = std::make_optional<sf::Text>(*m_font);
		m_good_font = true;
	}
	else
	{
		auto game = BabyDI::Get<tdrp::Game>();
		auto filepath = game->Server->FileSystem.GetFilePath(fs::FileCategory::ASSETS, name);
		if (filepath.empty())
			return;

		auto font = std::make_shared<sf::Font>();
		auto success = font->openFromFile(filepath);
		if (success)
		{
			m_good_font = true;
			m_font = font;
			m_text = std::make_optional<sf::Text>(*m_font);
			id = resources->Add(name, std::move(font));
		}
	}
}

void TextRenderComponent::load_text(const std::string& text)
{
	if (!m_text.has_value())
		return;

	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::TEXT)
			return;

		auto text_so = std::dynamic_pointer_cast<TextSceneObject>(so);
		if (!text_so)
			return;

		if (text_so->GetFont() != m_font_name)
			load_font(text_so->GetFont());
		m_text.value().setCharacterSize(text_so->GetFontSize());
	}

	m_text.value().setString(text);
}

void TextRenderComponent::property_update(uint16_t attribute_id)
{
	auto attribute = PropertyById(attribute_id);
	if (attribute != Property::TEXT)
		return;

	if (auto so = m_owner.lock())
	{
		if (so->GetType() != SceneObjectType::TEXT)
			return;

		auto text_so = std::dynamic_pointer_cast<TextSceneObject>(so);
		if (!text_so)
			return;

		load_text(so->GetText());
	}
}

std::any TextRenderComponent::provide_size() const
{
	auto box = GetBoundingBox();
	return std::make_any<Vector2df>(box.size);
}

std::any TextRenderComponent::provide_boundingbox() const
{
	auto box = GetBoundingBox();
	return std::make_any<Rectf>(box);
}

} // end namespace tdrp::component::render
