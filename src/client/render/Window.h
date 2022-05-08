#pragma once

#include <memory>

#include <SFML/Graphics.hpp>

#include "BabyDI.hpp"

#include "engine/filesystem/ProgramSettings.h"


namespace tdrp { class Game; }

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
	bool IsActive() const;

	void EventLoop();

private:
	INJECT(::tdrp::Game, Game);
	INJECT(::tdrp::settings::ProgramSettings, Settings);

	std::unique_ptr<sf::RenderWindow> m_window;
	bool m_window_active;
};

inline int32_t Window::GetWidth() const
{
	return m_window->getSize().x;
}

inline int32_t Window::GetHeight() const
{
	return m_window->getSize().y;
}

inline bool Window::IsActive() const
{
	return m_window_active;
}

} // end namespace tdrp::render
