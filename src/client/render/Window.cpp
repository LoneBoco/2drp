#include <stdio.h>
#include <cstdlib>

#include <SFML/Graphics/View.hpp>
#include <RmlUi/Debugger.h>

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
	//m_window->setFramerateLimit(60);
	m_window->setVerticalSyncEnabled(true);
	m_window->setKeyRepeatEnabled(false);

	sf::FloatRect visibleArea({ 0.f, 0.f }, { static_cast<float>(width), static_cast<float>(height) });
	m_window->setView(sf::View(visibleArea));
	Game->Camera.SetSize({ static_cast<int32_t>(width), static_cast<int32_t>(height) });
	
	Game->SetRenderWindow(m_window.get());
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

			else if (event.type == sf::Event::KeyPressed)
			{
				auto eat = Game->UI->ForEachVisible([&event](auto& context)
				{
					auto& si = Game->UI->SystemInterface;
					return context.ProcessKeyDown(si->TranslateKey(event.key.code), si->GetKeyModifiers());
				});
				if (eat) continue;

				if (event.key.code == sf::Keyboard::F8)
					Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());

				Game->OnKeyPress.RunAll(event.key.code);
			}

			else if (event.type == sf::Event::KeyReleased)
			{
				auto eat = Game->UI->ForEachVisible([&event](auto& context)
				{
					auto& si = Game->UI->SystemInterface;
					return context.ProcessKeyUp(si->TranslateKey(event.key.code), si->GetKeyModifiers());
				});
				if (eat) continue;
			}

			else if (event.type == sf::Event::TextEntered)
			{
				auto eat = Game->UI->ForEachVisible([&event](auto& context)
				{
					auto& si = Game->UI->SystemInterface;
					if (event.text.unicode > 32)
						return context.ProcessTextInput(Rml::Character(event.text.unicode));
					return false;
				});
				if (eat) continue;
			}

			else if (event.type == sf::Event::MouseMoved)
			{
				auto eat = Game->UI->ForEachVisible([&event](auto& context)
				{
					auto& si = Game->UI->SystemInterface;
					return context.ProcessMouseMove(event.mouseMove.x, event.mouseMove.y, si->GetKeyModifiers());
				});
				if (eat) continue;
			}

			else if (event.type == sf::Event::MouseWheelScrolled)
			{
				auto eat = Game->UI->ForEachVisible([&event](auto& context)
				{
					auto& si = Game->UI->SystemInterface;
					return context.ProcessMouseWheel(event.mouseWheelScroll.delta, si->GetKeyModifiers());
				});
				if (eat) continue;

				Vector2di pos{ event.mouseWheelScroll.x, event.mouseWheelScroll.y };
				Game->OnMouseWheel.RunAll(event.mouseWheelScroll.wheel, event.mouseWheelScroll.delta, pos);
			}

			else if (event.type == sf::Event::MouseButtonPressed)
			{
				auto eat = Game->UI->ForEachVisible([&event](auto& context)
				{
					auto& si = Game->UI->SystemInterface;
					return context.ProcessMouseButtonDown(event.mouseButton.button, si->GetKeyModifiers());
				});
				if (eat) continue;

				Vector2di pos{ event.mouseButton.x, event.mouseButton.y };
				Game->OnMouseDown.RunAll(event.mouseButton.button, pos);
			}

			else if (event.type == sf::Event::MouseButtonReleased)
			{
				auto eat = Game->UI->ForEachVisible([&event](auto& context)
				{
					auto& si = Game->UI->SystemInterface;
					return context.ProcessMouseButtonUp(event.mouseButton.button, si->GetKeyModifiers());
				});
				if (eat) continue;

				Vector2di pos{ event.mouseButton.x, event.mouseButton.y };
				Game->OnMouseUp.RunAll(event.mouseButton.button, pos);
			}
		}

		// If the camera is sized to our window, update it.
		if (Game->Camera.IsSizedToWindow())
		{
			const auto& view = m_window->getSize();
			Game->Camera.SetSize({ static_cast<int32_t>(view.x), static_cast<int32_t>(view.y) });
		}
		
		// Render start.
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
		Game->UI->Render();

		// Draw to the window.
		m_window->display();
	}
}

} // end namespace tdrp::render
