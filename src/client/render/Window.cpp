#include <stdio.h>
#include <cstdlib>

#include <SFML/Graphics/View.hpp>

#include "engine/common.h"
#include "engine/filesystem/ProgramSettings.h"

#include "client/render/Window.h"
#include "client/game/Game.h"

namespace tdrp::render
{

Window::Window(const char* title)
{
	auto width = Settings->GetAs<uint32_t>("window.width", 640);
	auto height = Settings->GetAs<uint32_t>("window.height", 480);

	sf::VideoMode vm{ width, height };

	m_window = std::make_unique<sf::RenderWindow>(vm, title);
	m_window->setFramerateLimit(60);

	sf::FloatRect visibleArea(0.f, 0.f, static_cast<float>(width), static_cast<float>(height));
	m_window->setView(sf::View(visibleArea));
	Game->Camera.SetSize({ static_cast<int32_t>(width), static_cast<int32_t>(height) });
}

Window::~Window()
{
}

void Window::EventLoop()
{
	while (m_window->isOpen())
	{
		sf::Event event;
		while (m_window->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				m_window->close();

			if (event.type == sf::Event::Resized)
			{
				//sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
				//m_window->setView(sf::View(visibleArea));
			}
		}

		// If the camera is sized to our window, update it.
		if (Game->Camera.IsSizedToWindow())
		{
			const auto& view = m_window->getSize();
			Game->Camera.SetSize({ static_cast<int32_t>(view.x), static_cast<int32_t>(view.y) });
		}
		
		// Set render camera position.
		const auto& camera = Game->Camera.GetViewWindow();
		sf::View view{
			{static_cast<float>(camera.pos.x), static_cast<float>(camera.pos.y)},
			{static_cast<float>(camera.size.x), static_cast<float>(camera.size.y)}
		};
		m_window->setView(view);

		// Render start.
		m_window->clear(sf::Color::Black);

		Game->Update();
		Game->Render(m_window.get());

		m_window->display();
	}
}

} // end namespace tdrp::render
