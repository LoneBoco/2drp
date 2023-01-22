#pragma once

#include <memory>

#include <SFML/Graphics.hpp>

#include "BabyDI.hpp"

#include "engine/scene/SceneObject.h"
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

	sf::RenderWindow* GetRenderWindow() const;
	int32_t GetWidth() const noexcept;
	int32_t GetHeight() const noexcept;
	bool IsActive() const noexcept;

	void EventLoop();

public:
	static void RenderPhysics(sf::RenderTarget&, SceneObjectPtr);

private:
	INJECT(::tdrp::Game, Game);
	INJECT(::tdrp::settings::ProgramSettings, Settings);

	std::unique_ptr<sf::RenderWindow> m_window;
	bool m_window_active;
};

inline sf::RenderWindow* Window::GetRenderWindow() const
{
	return m_window.get();
}

inline int32_t Window::GetWidth() const noexcept
{
	return m_window->getSize().x;
}

inline int32_t Window::GetHeight() const noexcept
{
	return m_window->getSize().y;
}

inline bool Window::IsActive() const noexcept
{
	return m_window_active;
}

} // end namespace tdrp::render
