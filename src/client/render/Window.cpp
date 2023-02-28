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

	sf::VideoMode vm{ width, height };

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

constinit auto rml_events = std::to_array<sf::Event::EventType>(
	{
		sf::Event::MouseMoved,
		sf::Event::MouseButtonPressed,
		sf::Event::MouseButtonReleased,
		sf::Event::MouseWheelScrolled,
		sf::Event::MouseLeft,
		sf::Event::TextEntered,
		sf::Event::KeyPressed,
		sf::Event::KeyReleased,
	}
);

void Window::EventLoop()
{
	while (m_window->isOpen())
	{
		// Handle events.
		sf::Event event;
		while (m_window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				m_window->close();

			else if (event.type == sf::Event::Resized)
			{
				Game->UI->ScreenSizeUpdate();
			}

			else if (event.type == sf::Event::LostFocus)
			{
				m_window_active = false;
			}

			else if (event.type == sf::Event::GainedFocus)
			{
				m_window_active = true;
			}

			else if (std::ranges::any_of(rml_events, [&event](auto& ev) { return ev == event.type; }))
			{
				auto eat = Game->UI->ForEachVisible([&event](auto& context) { return RmlSFML::InputHandler(&context, event); });
				if (eat) continue;

				if (event.type == sf::Event::KeyPressed)
				{
					if (event.key.code == sf::Keyboard::F8)
						Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());

					Game->OnKeyPress.RunAll(event.key.code);
				}
				else if (event.type == sf::Event::MouseWheelScrolled)
				{
					Vector2di pos{ event.mouseWheelScroll.x, event.mouseWheelScroll.y };
					Game->OnMouseWheel.RunAll(event.mouseWheelScroll.wheel, event.mouseWheelScroll.delta, pos);
				}
				else if (event.type == sf::Event::MouseButtonPressed)
				{
					Vector2di pos{ event.mouseButton.x, event.mouseButton.y };
					Game->OnMouseDown.RunAll(event.mouseButton.button, pos);
				}
				else if (event.type == sf::Event::MouseButtonReleased)
				{
					Vector2di pos{ event.mouseButton.x, event.mouseButton.y };
					Game->OnMouseUp.RunAll(event.mouseButton.button, pos);
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
	if (so->PhysicsBody.has_value())
	{
		auto scene = so->GetCurrentScene().lock();
		auto& world = scene->Physics.GetWorld();
		auto ppu = scene->Physics.GetPixelsPerUnit();

		const auto& body = playrho::d2::GetBody(world, so->PhysicsBody.value());
		if (!body.IsEnabled()) return;

		const auto& transformation = body.GetTransformation();

		auto shapes = playrho::d2::GetShapes(world, so->PhysicsBody.value());
		for (const auto& shapeid : shapes)
		{
			const auto& shape = playrho::d2::GetShape(world, shapeid);
			auto disk = playrho::d2::TypeCast<const playrho::d2::DiskShapeConf>(&shape);
			auto polygon = playrho::d2::TypeCast<const playrho::d2::PolygonShapeConf>(&shape);
			if (disk != nullptr)
			{
				auto radius = disk->GetRadius();
				auto location = disk->GetLocation();

				sf::CircleShape circle;
				circle.setFillColor(sf::Color::Transparent);
				circle.setOutlineColor(sf::Color::Yellow);
				circle.setOutlineThickness(1.0f);
				circle.setPosition({ (transformation.p[0] - radius + location[0]) * ppu, (transformation.p[1] - radius + location[1]) * ppu});
				circle.setRadius(radius * ppu);

				//window.resetGLStates();
				window.draw(circle);
			}
			else if (polygon != nullptr)
			{
				sf::ConvexShape poly;
				poly.setFillColor(sf::Color::Transparent);
				poly.setOutlineColor(sf::Color::Yellow);
				poly.setOutlineThickness(1.0f);

				poly.setPointCount(polygon->GetVertexCount());
				for (auto i = 0; i < polygon->GetVertexCount(); ++i)
				{
					auto vertex = polygon->GetVertex(i);
					poly.setPoint(i, { (transformation.p[0] + vertex[0]) * ppu, (transformation.p[1] + vertex[1]) * ppu });
				}

				//window.resetGLStates();
				window.draw(poly);
			}
		}
	}
}

} // end namespace tdrp::render
