#include <stdio.h>
#include <cstdlib>

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
				sf::FloatRect visibleArea(0.f, 0.f, event.size.width, event.size.height);
				m_window->setView(sf::View(visibleArea));
			}
		}

		m_window->clear(sf::Color::Black);

		Game->Update();
		Game->Render(m_window.get());

		m_window->display();
	}
}

} // end namespace tdrp::render
