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

#include "RmlUi_Backend.h"
#include "RmlUi_Platform_SFML.h"
#include "RmlUi_Renderer_GL2.h"
#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/Core.h>
#include <RmlUi/Core/FileInterface.h>
#include <RmlUi/Core/Profiling.h>
#include <RmlUi/Debugger/Debugger.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

/**
    Custom render interface example for the SFML/GL2 backend.

    Overloads the OpenGL2 render interface to load textures through SFML's built-in texture loading functionality.
 */
class RenderInterface_GL2_SFML : public RenderInterface_GL2 {
public:
	// -- Inherited from Rml::RenderInterface --

	void RenderGeometry(Rml::CompiledGeometryHandle handle, Rml::Vector2f translation, Rml::TextureHandle texture) override
	{
		if (texture)
		{
			sf::Texture::bind((sf::Texture*)texture);
			texture = RenderInterface_GL2::TextureEnableWithoutBinding;
		}

		RenderInterface_GL2::RenderGeometry(handle, translation, texture);
	}

	Rml::TextureHandle LoadTexture(Rml::Vector2i& texture_dimensions, const Rml::String& source) override
	{
		Rml::FileInterface* file_interface = Rml::GetFileInterface();
		Rml::FileHandle file_handle = file_interface->Open(source);
		if (!file_handle)
			return false;

		file_interface->Seek(file_handle, 0, SEEK_END);
		size_t buffer_size = file_interface->Tell(file_handle);
		file_interface->Seek(file_handle, 0, SEEK_SET);

		using Rml::byte;
		Rml::UniquePtr<byte[]> buffer(new byte[buffer_size]);
		file_interface->Read(buffer.get(), buffer_size, file_handle);
		file_interface->Close(file_handle);

		sf::Image image;
		if (!image.loadFromMemory(buffer.get(), buffer_size))
			return false;

		// Convert colors to premultiplied alpha, which is necessary for correct alpha compositing.
		for (unsigned int x = 0; x < image.getSize().x; x++)
		{
			for (unsigned int y = 0; y < image.getSize().y; y++)
			{
				sf::Color color = image.getPixel({ x, y });
				color.r = (uint8_t)((color.r * color.a) / 255);
				color.g = (uint8_t)((color.g * color.a) / 255);
				color.b = (uint8_t)((color.b * color.a) / 255);
				image.setPixel({ x, y }, color);
			}
		}

		sf::Texture* texture = new sf::Texture();
		texture->setSmooth(true);

		if (!texture->loadFromImage(image))
		{
			delete texture;
			return false;
		}

		texture_dimensions = Rml::Vector2i(texture->getSize().x, texture->getSize().y);
		return (Rml::TextureHandle)texture;
	}

	Rml::TextureHandle GenerateTexture(Rml::Span<const Rml::byte> source, Rml::Vector2i source_dimensions) override
	{
		sf::Texture* texture = new sf::Texture();
		texture->setSmooth(true);

		if (!texture->resize({ static_cast<uint32_t>(source_dimensions.x), static_cast<uint32_t>(source_dimensions.y) }))
		{
			delete texture;
			return false;
		}

		texture->update(source.data(), { static_cast<uint32_t>(source_dimensions.x), static_cast<uint32_t>(source_dimensions.y) }, { 0, 0 });

		return (Rml::TextureHandle)texture;
	}

	void ReleaseTexture(Rml::TextureHandle texture_handle) override { delete (sf::Texture*)texture_handle; }
};

// Updates the viewport and context dimensions, should be called whenever the window size changes.
static void UpdateWindowDimensions(sf::RenderWindow& window, RenderInterface_GL2_SFML& render_interface, Rml::Context* context)
{
	const int width = (int)window.getSize().x;
	const int height = (int)window.getSize().y;

	if (context)
		context->SetDimensions(Rml::Vector2i(width, height));

	sf::View view(sf::FloatRect({ 0.f, 0.f }, { (float)width, (float)height }));
	window.setView(view);

	render_interface.SetViewport(width, height);
}

/**
    Global data used by this backend.

    Lifetime governed by the calls to Backend::Initialize() and Backend::Shutdown().
 */
struct BackendData {
	SystemInterface_SFML system_interface;
	RenderInterface_GL2_SFML render_interface;
	sf::RenderWindow* window;
	bool running = true;
};
static Rml::UniquePtr<BackendData> data;

bool Backend::Initialize(sf::RenderWindow* window)
{
	RMLUI_ASSERT(!data);

	data = Rml::MakeUnique<BackendData>();

	data->window = window;
	data->system_interface.SetWindow(data->window);

	UpdateWindowDimensions(*data->window, data->render_interface, nullptr);

	return true;
}

void Backend::Shutdown()
{
	data.reset();
}

Rml::SystemInterface* Backend::GetSystemInterface()
{
	RMLUI_ASSERT(data);
	return &data->system_interface;
}

Rml::RenderInterface* Backend::GetRenderInterface()
{
	RMLUI_ASSERT(data);
	return &data->render_interface;
}

bool Backend::ProcessEvents(Rml::Context* context, KeyDownCallback key_down_callback, bool power_save)
{
	RMLUI_ASSERT(data && context && data->window);

	// SFML does not seem to provide a way to wait for events with a timeout.
	(void)power_save;

	// The contents of this function is intended to be copied directly into your main loop.
	bool result = data->running;
	data->running = true;

	while (const std::optional event = data->window->pollEvent())
	{
		if (const auto* ev = event->getIf<sf::Event::Resized>(); ev)
		{
			UpdateWindowDimensions(*data->window, data->render_interface, context);
		}
		else if (const auto* ev = event->getIf<sf::Event::KeyPressed>(); ev)
		{
			const Rml::Input::KeyIdentifier key = RmlSFML::ConvertKey(ev->code);
			const int key_modifier = RmlSFML::GetKeyModifierState();
			const float native_dp_ratio = 1.f;

			// See if we have any global shortcuts that take priority over the context.
			if (key_down_callback && !key_down_callback(context, key, key_modifier, native_dp_ratio, true))
				break;
			// Otherwise, hand the event over to the context by calling the input handler as normal.
			if (!RmlSFML::InputHandler(context, *ev))
				break;
			// The key was not consumed by the context either, try keyboard shortcuts of lower priority.
			if (key_down_callback && !key_down_callback(context, key, key_modifier, native_dp_ratio, false))
				break;
		}
		else if (event->is<sf::Event::Closed>())
		{
			result = false;
		}
		else
		{
			RmlSFML::InputHandler(context, *event);
		}
	}

	return result;
}

void Backend::RequestExit()
{
	RMLUI_ASSERT(data);
	data->running = false;
}
	
void Backend::BeginFrame()
{
	RMLUI_ASSERT(data);
	sf::RenderWindow& window = *data->window;

	window.resetGLStates();
	window.clear();

	data->render_interface.BeginFrame();
}

void Backend::PresentFrame()
{
	RMLUI_ASSERT(data);

	data->render_interface.EndFrame();
	data->window->display();

	// Optional, used to mark frames during performance profiling.
	RMLUI_FrameMark;
}
