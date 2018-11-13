#pragma once

#include <memory>

#include <SFML/Graphics.hpp>

#include "BabyDI.hpp"

namespace tdrp
{
	class Game;
}

namespace tdrp::settings
{
	class ProgramSettings;
}

namespace tdrp::render
{

class Window
{
public:
	Window(const char* title);
	Window(const Window& other) = delete;
	Window(Window&& other) = delete;

	~Window();

	Window& operator=(const Window& other) = delete;

	int32_t GetWidth() const;
	int32_t GetHeight() const;

	void EventLoop();

private:
	BabyDI::Injected<::tdrp::Game> Game;
	BabyDI::Injected<tdrp::settings::ProgramSettings> Settings;

	std::unique_ptr<sf::RenderWindow> m_window;
};

inline int32_t Window::GetWidth() const
{
	return m_window->getSize().x;
}

inline int32_t Window::GetHeight() const
{
	return m_window->getSize().y;
}

} // end namespace tdrp::render
