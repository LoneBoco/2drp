#include <stdio.h>
#include <cstdlib>

#include <SFML/Graphics/View.hpp>
#include <RmlUi/Debugger.h>

#include "engine/common.h"
#include "engine/filesystem/ProgramSettings.h"
#include "engine/filesystem/Log.h"

#include "client/render/Window.h"
#include "client/game/Game.h"

#include "client/ui/interface/RmlUi_Platform_SFML.h"

namespace tdrp::render
{

Window::Window(const char* title)
{
	auto width = Settings->GetAs<uint32_t>("window.width", 640);
	auto height = Settings->GetAs<uint32_t>("window.height", 480);

	sf::VideoMode vm{ { width, height } };

	m_window = std::make_unique<sf::RenderWindow>(vm, title);
	//m_window->setFramerateLimit(60);
	m_window->setVerticalSyncEnabled(true);
	m_window->setKeyRepeatEnabled(false);

	sf::FloatRect visibleArea({ 0.f, 0.f }, { static_cast<float>(width), static_cast<float>(height) });
	m_window->setView(sf::View(visibleArea));
	Game->Camera.SetSize({ static_cast<int32_t>(width), static_cast<int32_t>(height) });
}

Window::~Window()
{
	log::PrintLine(":: Closing window.");
}

constinit auto rml_events = std::variant<
	sf::Event::MouseMoved,
	sf::Event::MouseButtonPressed,
	sf::Event::MouseButtonReleased,
	sf::Event::MouseWheelScrolled,
	sf::Event::MouseLeft,
	sf::Event::TextEntered,
	sf::Event::KeyPressed,
	sf::Event::KeyReleased
>{};

template <typename... Ts>
[[nodiscard]] static constexpr bool isRmlEvent(const sf::Event& ev, const std::variant<Ts...>&)
{
	return (ev.is<Ts>() || ...);
}

void Window::EventLoop()
{
	while (m_window->isOpen())
	{
		// Handle events.
		while (const std::optional event = m_window->pollEvent())
		{
			if (!event.has_value())
				continue;

			if (event->is<sf::Event::Closed>())
				m_window->close();

			else if (event->is<sf::Event::Resized>())
			{
				Game->UI->ScreenSizeUpdate();
			}

			else if (event->is<sf::Event::FocusLost>())
			{
				m_window_active = false;
			}

			else if (event->is<sf::Event::FocusGained>())
			{
				m_window_active = true;
			}

			else if (isRmlEvent(event.value(), rml_events))
			{
				auto eat = Game->UI->ForEachVisible([&event](auto& context) { return RmlSFML::InputHandler(&context, event.value()); });
				if (eat) continue;

				if (const auto* ev = event->getIf<sf::Event::KeyPressed>(); ev)
				{
					if (ev->code == sf::Keyboard::Key::F8)
						Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());

					Game->OnKeyPress.RunAll(ev->code);
				}
				else if (const auto* ev = event->getIf<sf::Event::MouseWheelScrolled>(); ev)
				{
					Vector2di pos{ ev->position.x, ev->position.y };
					Game->OnMouseWheel.RunAll((int)ev->wheel, ev->delta, pos);
				}
				else if (const auto* ev = event->getIf<sf::Event::MouseButtonPressed>(); ev)
				{
					Vector2di pos{ ev->position.x, ev->position.y };
					Game->OnMouseDown.RunAll((int)ev->button, pos);
				}
				else if (const auto* ev = event->getIf < sf::Event::MouseButtonReleased>(); ev)
				{
					Vector2di pos{ ev->position.x, ev->position.y };
					Game->OnMouseUp.RunAll((int)ev->button, pos);
				}
			}
		}

		// If the camera is sized to our window, update it.
		if (Game->Camera.IsSizedToWindow())
		{
			const auto& view = m_window->getSize();
			Game->Camera.SetSize({ static_cast<int32_t>(view.x), static_cast<int32_t>(view.y) });
		}
		
		// Render start.
		m_window->resetGLStates();
		m_window->clear(sf::Color::Black);

		// Update the game / server.
		Game->Update();

		// Set render camera position.
		// This will have been changed via script.
		const auto& camera = Game->Camera.GetCamera();
		sf::View view{
			{static_cast<float>(camera.pos.x), static_cast<float>(camera.pos.y)},
			{static_cast<float>(camera.size.x), static_cast<float>(camera.size.y)}
		};
		m_window->setView(view);

		// Update the UI.
		Game->UI->Update();

		// Render the game.
		Game->Render(m_window.get());
		m_window->resetGLStates();
		Game->UI->Render();

		// Draw to the window.
		m_window->display();
	}
}

void Window::RenderPhysics(sf::RenderTarget& window, SceneObjectPtr so)
{
	auto scene = so->GetCurrentScene().lock();
	auto& world = scene->Physics.GetWorld();
	auto ppu = scene->Physics.GetPixelsPerUnit();

	if (so->HasPhysicsBody())
	{
		const auto& body_id = so->GetPhysicsBody().value();
		if (!playrho::d2::IsEnabled(world, body_id))
			return;

		auto transformation = playrho::d2::GetTransformation(world, body_id);
		auto fixtures = playrho::d2::GetFixtures(world, body_id);
		for (const auto& fixture_id : fixtures)
		{
			const auto& shape = playrho::d2::GetShape(world, fixture_id);
			auto disk = playrho::d2::TypeCast<const playrho::d2::DiskShapeConf>(&shape);
			auto polygon = playrho::d2::TypeCast<const playrho::d2::PolygonShapeConf>(&shape);
			if (disk != nullptr)
			{
				auto radius = disk->GetRadius();
				auto location = disk->GetLocation();

				sf::CircleShape circle;
				circle.setFillColor(sf::Color::Transparent);
				circle.setOutlineThickness(1.0f);
				circle.setPosition({ (transformation.p[0] - radius + location[0]) * ppu, (transformation.p[1] - radius + location[1]) * ppu });
				circle.setRadius(radius * ppu);

				auto filter = playrho::d2::GetFilterData(world, fixture_id);
				if (filter.categoryBits == physics::category_default)
					circle.setOutlineColor(sf::Color::Yellow);
				else if (filter.categoryBits == physics::category_hybrid)
					circle.setOutlineColor(sf::Color::Cyan);

				//window.resetGLStates();
				window.draw(circle);
			}
			else if (polygon != nullptr)
			{
				sf::ConvexShape poly;
				poly.setPointCount(polygon->GetVertexCount());
				for (auto i = 0; i < polygon->GetVertexCount(); ++i)
				{
					auto vertex = polygon->GetVertex(i);
					poly.setPoint(i, { (transformation.p[0] + vertex[0]) * ppu, (transformation.p[1] + vertex[1]) * ppu });
				}

				poly.setFillColor(sf::Color::Transparent);
				poly.setOutlineColor(sf::Color::Yellow);
				poly.setOutlineThickness(1.0f);

				//window.resetGLStates();
				window.draw(poly);
			}
		}
	}
}

} // end namespace tdrp::render
