/*
 * This source file is part of RmlUi, the HTML/CSS Interface Middleware
 *
 * For the latest information, see http://github.com/mikke89/RmlUi
 *
 * Copyright (c) 2008-2010 CodePoint Ltd, Shift Technology Ltd
 * Copyright (c) 2019 The RmlUi Team, and contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "RmlUi_Platform_SFML.h"
#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/Input.h>
#include <RmlUi/Core/StringUtilities.h>
#include <RmlUi/Core/SystemInterface.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

SystemInterface_SFML::SystemInterface_SFML()
{
	cursor_resize = sf::Cursor::createFromSystem(sf::Cursor::Type::SizeTopLeftBottomRight);
	if (!cursor_resize.has_value())
		cursor_resize = sf::Cursor::createFromSystem(sf::Cursor::Type::SizeAll);
}

void SystemInterface_SFML::SetWindow(sf::RenderWindow* in_window)
{
	window = in_window;
}

double SystemInterface_SFML::GetElapsedTime()
{
	return static_cast<double>(timer.getElapsedTime().asMicroseconds()) / 1'000'000.0;
}

void SystemInterface_SFML::SetMouseCursor(const Rml::String& cursor_name)
{
	if (window)
	{
		sf::Cursor* cursor = nullptr;
		if (cursor_name.empty() || cursor_name == "arrow")
			cursor = &cursor_default;
		else if (cursor_name == "move")
			cursor = &cursor_move;
		else if (cursor_name == "pointer")
			cursor = &cursor_pointer;
		else if (cursor_name == "resize" && cursor_resize.has_value())
			cursor = &cursor_resize.value();
		else if (cursor_name == "cross")
			cursor = &cursor_cross;
		else if (cursor_name == "text")
			cursor = &cursor_text;
		else if (cursor_name == "unavailable")
			cursor = &cursor_unavailable;
		else if (Rml::StringUtilities::StartsWith(cursor_name, "rmlui-scroll"))
			cursor = &cursor_move;

		if (cursor)
			window->setMouseCursor(*cursor);
	}
}

void SystemInterface_SFML::SetClipboardText(const Rml::String& text_utf8)
{
	sf::Clipboard::setString(text_utf8);
}

void SystemInterface_SFML::GetClipboardText(Rml::String& text)
{
	text = sf::Clipboard::getString();
}

bool RmlSFML::InputHandler(Rml::Context* context, const sf::Event& event)
{
	if (const auto* ev = event.getIf<sf::Event::MouseMoved>())
	{
		return context->ProcessMouseMove(ev->position.x, ev->position.y, RmlSFML::GetKeyModifierState());
	}
	else if (const auto* ev = event.getIf<sf::Event::MouseButtonPressed>())
	{
		return context->ProcessMouseButtonDown((int)ev->button, RmlSFML::GetKeyModifierState());
	}
	else if (const auto* ev = event.getIf<sf::Event::MouseButtonReleased>())
	{
		return context->ProcessMouseButtonUp((int)ev->button, RmlSFML::GetKeyModifierState());
	}
	else if (const auto* ev = event.getIf<sf::Event::MouseWheelScrolled>())
	{
		return context->ProcessMouseWheel(float(-ev->delta), RmlSFML::GetKeyModifierState());
	}
	else if (event.is<sf::Event::MouseLeft>())
	{
		return context->ProcessMouseLeave();
	}
	else if (const auto* ev = event.getIf<sf::Event::TextEntered>())
	{
		Rml::Character character = Rml::Character(ev->unicode);
		if (character == Rml::Character('\r'))
			character = Rml::Character('\n');

		if (ev->unicode >= 32 || character == Rml::Character('\n'))
			return context->ProcessTextInput(character);
	}
	else if (const auto* ev = event.getIf<sf::Event::KeyPressed>())
	{
		return context->ProcessKeyDown(RmlSFML::ConvertKey(ev->code), RmlSFML::GetKeyModifierState());
	}
	else if (const auto* ev = event.getIf<sf::Event::KeyReleased>())
	{
		return context->ProcessKeyUp(RmlSFML::ConvertKey(ev->code), RmlSFML::GetKeyModifierState());
	}

	return false;
}

Rml::Input::KeyIdentifier RmlSFML::ConvertKey(sf::Keyboard::Key sfml_key)
{
	// clang-format off
	switch (sfml_key)
	{
		case sf::Keyboard::Key::A:         return Rml::Input::KI_A;
		case sf::Keyboard::Key::B:         return Rml::Input::KI_B;
		case sf::Keyboard::Key::C:         return Rml::Input::KI_C;
		case sf::Keyboard::Key::D:         return Rml::Input::KI_D;
		case sf::Keyboard::Key::E:         return Rml::Input::KI_E;
		case sf::Keyboard::Key::F:         return Rml::Input::KI_F;
		case sf::Keyboard::Key::G:         return Rml::Input::KI_G;
		case sf::Keyboard::Key::H:         return Rml::Input::KI_H;
		case sf::Keyboard::Key::I:         return Rml::Input::KI_I;
		case sf::Keyboard::Key::J:         return Rml::Input::KI_J;
		case sf::Keyboard::Key::K:         return Rml::Input::KI_K;
		case sf::Keyboard::Key::L:         return Rml::Input::KI_L;
		case sf::Keyboard::Key::M:         return Rml::Input::KI_M;
		case sf::Keyboard::Key::N:         return Rml::Input::KI_N;
		case sf::Keyboard::Key::O:         return Rml::Input::KI_O;
		case sf::Keyboard::Key::P:         return Rml::Input::KI_P;
		case sf::Keyboard::Key::Q:         return Rml::Input::KI_Q;
		case sf::Keyboard::Key::R:         return Rml::Input::KI_R;
		case sf::Keyboard::Key::S:         return Rml::Input::KI_S;
		case sf::Keyboard::Key::T:         return Rml::Input::KI_T;
		case sf::Keyboard::Key::U:         return Rml::Input::KI_U;
		case sf::Keyboard::Key::V:         return Rml::Input::KI_V;
		case sf::Keyboard::Key::W:         return Rml::Input::KI_W;
		case sf::Keyboard::Key::X:         return Rml::Input::KI_X;
		case sf::Keyboard::Key::Y:         return Rml::Input::KI_Y;
		case sf::Keyboard::Key::Z:         return Rml::Input::KI_Z;
		case sf::Keyboard::Key::Num0:      return Rml::Input::KI_0;
		case sf::Keyboard::Key::Num1:      return Rml::Input::KI_1;
		case sf::Keyboard::Key::Num2:      return Rml::Input::KI_2;
		case sf::Keyboard::Key::Num3:      return Rml::Input::KI_3;
		case sf::Keyboard::Key::Num4:      return Rml::Input::KI_4;
		case sf::Keyboard::Key::Num5:      return Rml::Input::KI_5;
		case sf::Keyboard::Key::Num6:      return Rml::Input::KI_6;
		case sf::Keyboard::Key::Num7:      return Rml::Input::KI_7;
		case sf::Keyboard::Key::Num8:      return Rml::Input::KI_8;
		case sf::Keyboard::Key::Num9:      return Rml::Input::KI_9;
		case sf::Keyboard::Key::Numpad0:   return Rml::Input::KI_NUMPAD0;
		case sf::Keyboard::Key::Numpad1:   return Rml::Input::KI_NUMPAD1;
		case sf::Keyboard::Key::Numpad2:   return Rml::Input::KI_NUMPAD2;
		case sf::Keyboard::Key::Numpad3:   return Rml::Input::KI_NUMPAD3;
		case sf::Keyboard::Key::Numpad4:   return Rml::Input::KI_NUMPAD4;
		case sf::Keyboard::Key::Numpad5:   return Rml::Input::KI_NUMPAD5;
		case sf::Keyboard::Key::Numpad6:   return Rml::Input::KI_NUMPAD6;
		case sf::Keyboard::Key::Numpad7:   return Rml::Input::KI_NUMPAD7;
		case sf::Keyboard::Key::Numpad8:   return Rml::Input::KI_NUMPAD8;
		case sf::Keyboard::Key::Numpad9:   return Rml::Input::KI_NUMPAD9;
		case sf::Keyboard::Key::Left:      return Rml::Input::KI_LEFT;
		case sf::Keyboard::Key::Right:     return Rml::Input::KI_RIGHT;
		case sf::Keyboard::Key::Up:        return Rml::Input::KI_UP;
		case sf::Keyboard::Key::Down:      return Rml::Input::KI_DOWN;
		case sf::Keyboard::Key::Add:       return Rml::Input::KI_ADD;
		case sf::Keyboard::Key::Backspace: return Rml::Input::KI_BACK;
		case sf::Keyboard::Key::Delete:    return Rml::Input::KI_DELETE;
		case sf::Keyboard::Key::Divide:    return Rml::Input::KI_DIVIDE;
		case sf::Keyboard::Key::End:       return Rml::Input::KI_END;
		case sf::Keyboard::Key::Escape:    return Rml::Input::KI_ESCAPE;
		case sf::Keyboard::Key::F1:        return Rml::Input::KI_F1;
		case sf::Keyboard::Key::F2:        return Rml::Input::KI_F2;
		case sf::Keyboard::Key::F3:        return Rml::Input::KI_F3;
		case sf::Keyboard::Key::F4:        return Rml::Input::KI_F4;
		case sf::Keyboard::Key::F5:        return Rml::Input::KI_F5;
		case sf::Keyboard::Key::F6:        return Rml::Input::KI_F6;
		case sf::Keyboard::Key::F7:        return Rml::Input::KI_F7;
		case sf::Keyboard::Key::F8:        return Rml::Input::KI_F8;
		case sf::Keyboard::Key::F9:        return Rml::Input::KI_F9;
		case sf::Keyboard::Key::F10:       return Rml::Input::KI_F10;
		case sf::Keyboard::Key::F11:       return Rml::Input::KI_F11;
		case sf::Keyboard::Key::F12:       return Rml::Input::KI_F12;
		case sf::Keyboard::Key::F13:       return Rml::Input::KI_F13;
		case sf::Keyboard::Key::F14:       return Rml::Input::KI_F14;
		case sf::Keyboard::Key::F15:       return Rml::Input::KI_F15;
		case sf::Keyboard::Key::Home:      return Rml::Input::KI_HOME;
		case sf::Keyboard::Key::Insert:    return Rml::Input::KI_INSERT;
		case sf::Keyboard::Key::LControl:  return Rml::Input::KI_LCONTROL;
		case sf::Keyboard::Key::LShift:    return Rml::Input::KI_LSHIFT;
		case sf::Keyboard::Key::Multiply:  return Rml::Input::KI_MULTIPLY;
		case sf::Keyboard::Key::Pause:     return Rml::Input::KI_PAUSE;
		case sf::Keyboard::Key::RControl:  return Rml::Input::KI_RCONTROL;
		case sf::Keyboard::Key::Enter:     return Rml::Input::KI_RETURN;
		case sf::Keyboard::Key::RShift:    return Rml::Input::KI_RSHIFT;
		case sf::Keyboard::Key::Space:     return Rml::Input::KI_SPACE;
		case sf::Keyboard::Key::Subtract:  return Rml::Input::KI_SUBTRACT;
		case sf::Keyboard::Key::Tab:       return Rml::Input::KI_TAB;
		default: break;
	}
	// clang-format on

	return Rml::Input::KI_UNKNOWN;
}

int RmlSFML::GetKeyModifierState()
{
	int modifiers = 0;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RShift))
		modifiers |= Rml::Input::KM_SHIFT;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RControl))
		modifiers |= Rml::Input::KM_CTRL;

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LAlt) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::RAlt))
		modifiers |= Rml::Input::KM_ALT;

	return modifiers;
}
