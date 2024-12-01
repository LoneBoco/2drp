#include <SFML/Window.hpp>

#include "client/game/Game.h"
#include "client/script/Script.h"

// Put these two together to resolve an undefined type error.
#include "client/render/Window.h"
#include "engine/filesystem/ProgramSettings.h"
//

#include "engine/scene/SceneObject.h"
//#include "engine/network/Packet.h"
//#include "engine/network/PacketsInc.h"

//using tdrp::network::PACKETID;
//using tdrp::network::Packets;

namespace tdrp
{

bool keydown(int key);
bool keyup(int key);
void logF(const char* message);
// void playSound(const std::string& sound);
// void playSound(const std::string& sound, const Vector2df& position);

void bind_client(sol::state& lua)
{
	bind_globals(lua);
	bind_camera(lua);
	bind_game(lua);
}

void bind_globals(sol::state& lua)
{
	// Bind keyboard keys.
	lua.new_enum<sf::Keyboard::Key>("Key", {
		  { "Unknown", sf::Keyboard::Key::Unknown }
		, { "A", sf::Keyboard::Key::A }
		, { "B", sf::Keyboard::Key::B }
		, { "C", sf::Keyboard::Key::C }
		, { "D", sf::Keyboard::Key::D }
		, { "E", sf::Keyboard::Key::E }
		, { "F", sf::Keyboard::Key::F }
		, { "G", sf::Keyboard::Key::G }
		, { "H", sf::Keyboard::Key::H }
		, { "I", sf::Keyboard::Key::I }
		, { "J", sf::Keyboard::Key::J }
		, { "K", sf::Keyboard::Key::K }
		, { "L", sf::Keyboard::Key::L }
		, { "M", sf::Keyboard::Key::M }
		, { "N", sf::Keyboard::Key::N }
		, { "O", sf::Keyboard::Key::O }
		, { "P", sf::Keyboard::Key::P }
		, { "Q", sf::Keyboard::Key::Q }
		, { "R", sf::Keyboard::Key::R }
		, { "S", sf::Keyboard::Key::S }
		, { "T", sf::Keyboard::Key::T }
		, { "U", sf::Keyboard::Key::U }
		, { "V", sf::Keyboard::Key::V }
		, { "W", sf::Keyboard::Key::W }
		, { "X", sf::Keyboard::Key::X }
		, { "Y", sf::Keyboard::Key::Y }
		, { "Z", sf::Keyboard::Key::Z }
		, { "Num0", sf::Keyboard::Key::Num0 }
		, { "Num1", sf::Keyboard::Key::Num1 }
		, { "Num2", sf::Keyboard::Key::Num2 }
		, { "Num3", sf::Keyboard::Key::Num3 }
		, { "Num4", sf::Keyboard::Key::Num4 }
		, { "Num5", sf::Keyboard::Key::Num5 }
		, { "Num6", sf::Keyboard::Key::Num6 }
		, { "Num7", sf::Keyboard::Key::Num7 }
		, { "Num8", sf::Keyboard::Key::Num8 }
		, { "Num9", sf::Keyboard::Key::Num9 }
		, { "Escape", sf::Keyboard::Key::Escape }
		, { "LControl", sf::Keyboard::Key::LControl }
		, { "LShift", sf::Keyboard::Key::LShift }
		, { "LAlt", sf::Keyboard::Key::LAlt }
		, { "LSystem", sf::Keyboard::Key::LSystem }
		, { "RControl", sf::Keyboard::Key::RControl }
		, { "RShift", sf::Keyboard::Key::RShift }
		, { "RAlt", sf::Keyboard::Key::RAlt }
		, { "RSystem", sf::Keyboard::Key::RSystem }
		, { "Menu", sf::Keyboard::Key::Menu }
		, { "LBracket", sf::Keyboard::Key::LBracket }
		, { "RBracket", sf::Keyboard::Key::RBracket }
		, { "Semicolon", sf::Keyboard::Key::Semicolon }
		, { "Comma", sf::Keyboard::Key::Comma }
		, { "Period", sf::Keyboard::Key::Period }
		, { "Apostrophe", sf::Keyboard::Key::Apostrophe }
		, { "Quote", sf::Keyboard::Key::Apostrophe }
		, { "Slash", sf::Keyboard::Key::Slash }
		, { "Backslash", sf::Keyboard::Key::Backslash }
		, { "Grave", sf::Keyboard::Key::Grave }
		, { "Tilde", sf::Keyboard::Key::Grave }
		, { "Equal", sf::Keyboard::Key::Equal }
		, { "Hyphen", sf::Keyboard::Key::Hyphen }
		, { "Space", sf::Keyboard::Key::Space }
		, { "Enter", sf::Keyboard::Key::Enter }
		, { "Backspace", sf::Keyboard::Key::Backspace }
		, { "Tab", sf::Keyboard::Key::Tab }
		, { "PageUp", sf::Keyboard::Key::PageUp }
		, { "PageDown", sf::Keyboard::Key::PageDown }
		, { "End", sf::Keyboard::Key::End }
		, { "Home", sf::Keyboard::Key::Home }
		, { "Insert", sf::Keyboard::Key::Insert }
		, { "Delete", sf::Keyboard::Key::Delete }
		, { "Add", sf::Keyboard::Key::Add }
		, { "Subtract", sf::Keyboard::Key::Subtract }
		, { "Multiply", sf::Keyboard::Key::Multiply }
		, { "Divide", sf::Keyboard::Key::Divide }
		, { "Left", sf::Keyboard::Key::Left }
		, { "Right", sf::Keyboard::Key::Right }
		, { "Up", sf::Keyboard::Key::Up }
		, { "Down", sf::Keyboard::Key::Down }
		, { "Numpad0", sf::Keyboard::Key::Numpad0 }
		, { "Numpad1", sf::Keyboard::Key::Numpad1 }
		, { "Numpad2", sf::Keyboard::Key::Numpad2 }
		, { "Numpad3", sf::Keyboard::Key::Numpad3 }
		, { "Numpad4", sf::Keyboard::Key::Numpad4 }
		, { "Numpad5", sf::Keyboard::Key::Numpad5 }
		, { "Numpad6", sf::Keyboard::Key::Numpad6 }
		, { "Numpad7", sf::Keyboard::Key::Numpad7 }
		, { "Numpad8", sf::Keyboard::Key::Numpad8 }
		, { "Numpad9", sf::Keyboard::Key::Numpad9 }
		, { "F1", sf::Keyboard::Key::F1 }
		, { "F2", sf::Keyboard::Key::F2 }
		, { "F3", sf::Keyboard::Key::F3 }
		, { "F4", sf::Keyboard::Key::F4 }
		, { "F5", sf::Keyboard::Key::F5 }
		, { "F6", sf::Keyboard::Key::F6 }
		, { "F7", sf::Keyboard::Key::F7 }
		, { "F8", sf::Keyboard::Key::F8 }
		, { "F9", sf::Keyboard::Key::F9 }
		, { "F10", sf::Keyboard::Key::F10 }
		, { "F11", sf::Keyboard::Key::F11 }
		, { "F12", sf::Keyboard::Key::F12 }
		, { "F13", sf::Keyboard::Key::F13 }
		, { "F14", sf::Keyboard::Key::F14 }
		, { "F15", sf::Keyboard::Key::F15 }
		, { "Pause", sf::Keyboard::Key::Pause }
	});

	lua.new_enum<sf::Mouse::Wheel>("MouseWheel", {
		  { "Vertical", sf::Mouse::Wheel::Vertical }
		, { "Horizontal", sf::Mouse::Wheel::Horizontal }
	});

	lua.new_enum<sf::Mouse::Button>("MouseButton", {
		  { "Left", sf::Mouse::Button::Left }
		, { "Right", sf::Mouse::Button::Right }
		, { "Middle", sf::Mouse::Button::Middle }
		, { "XButton1", sf::Mouse::Button::Extra1 }
		, { "XButton2", sf::Mouse::Button::Extra2 }
		, { "Extra1", sf::Mouse::Button::Extra1 }
		, { "Extra2", sf::Mouse::Button::Extra2 }
	});

	/*
	lua.set_function("playSound", sol::overload(
		sol::resolve<void(const std::string&)>(playSound),
		sol::resolve<void(const std::string&, const Vector2df&)>(playSound)
	));
	*/
}

void bind_camera(sol::state& lua)
{
	lua.new_usertype<camera::Camera>("Camera", sol::no_constructor,
		"Position", sol::property(&camera::Camera::GetPosition, &camera::Camera::LookAt),
		"LerpTo", [&](camera::Camera& camera, const Vector2di& position, uint32_t ms) { camera.LerpTo(position, std::chrono::milliseconds(ms)); },
		"FollowSceneObject", sol::overload(
			[&](camera::Camera& camera, std::shared_ptr<SceneObject>& sceneobject) { camera.FollowSceneObject(sceneobject); },
			[&](camera::Camera& camera, std::shared_ptr<SceneObject>& sceneobject, uint32_t ms) { camera.FollowSceneObject(sceneobject, std::chrono::milliseconds(ms)); }
		),
		"FollowOffset", sol::property(&camera::Camera::GetFollowOffset, &camera::Camera::SetFollowOffset),
		"Size", sol::property(&camera::Camera::GetSize, &camera::Camera::SetSize),
		"SizeToWindow", &camera::Camera::SizeToWindow,
		"ViewRect", sol::readonly_property(&camera::Camera::GetViewRect),
		"IsSizedToWindow", &camera::Camera::IsSizedToWindow
	);
}

namespace functions
{
	static ObjectAttributes* GetFlags(Game& self)
	{
		auto player = self.GetCurrentPlayer();
		if (player == nullptr) return nullptr;

		return &player->Account.Flags;
	}
}

void bind_game(sol::state& lua)
{

	lua.new_usertype<Game>("Game", sol::no_constructor,
		"log", &logF,
		"keydown", &keydown,
		"keyup", &keyup,

		"SendEvent", &Game::SendEvent,
		"SendServerEvent", &Game::SendServerEvent,
		"SendLevelEvent", &Game::SendLevelEvent,

		"ToggleUIContext", [](Game& self, const std::string& context, bool visible) { self.UI->ToggleContextVisibility(context, visible); },
		"ToggleUIDocument", [](Game& self, const std::string& context, const std::string& document) { self.UI->ToggleDocumentVisibility(context, document); },
		"ReloadUI", [](Game& self) { self.UI->ReloadUI(); },
		"AssignDebugger", [](Game& self, const std::string& context) { self.UI->AssignDebugger(context); },

		"Camera", sol::readonly_property(&Game::Camera),
		"Player", sol::readonly_property(&Game::GetCurrentPlayer),
		"Server", sol::readonly_property(&Game::Server),
		"Flags", sol::readonly_property(&functions::GetFlags),

		"OnCreated", sol::writeonly_property(&Game::SetOnCreated),
		"OnDestroyed", sol::writeonly_property(&Game::SetOnDestroyed),
		"OnConnected", sol::writeonly_property(&Game::SetOnConnected),
		"OnClientFrame", sol::writeonly_property(&Game::SetOnClientFrame),
		"OnKeyPress", sol::writeonly_property(&Game::SetOnKeyPress),
		"OnMouseWheel", sol::writeonly_property(&Game::SetOnMouseWheel),
		"OnMouseDown", sol::writeonly_property(&Game::SetOnMouseDown),
		"OnMouseUp", sol::writeonly_property(&Game::SetOnMouseUp),
		"OnGainedOwnership", sol::writeonly_property(&Game::SetOnGainedOwnership),
		"OnSceneSwitch", sol::writeonly_property(&Game::SetOnSceneSwitch),
		"OnItemsUpdate", sol::writeonly_property(&Game::SetOnItemsUpdate)
	);
}

///////////////////////////////////////////////////////////////////////////////

bool keydown(int key)
{
	auto window = BabyDI::Get<render::Window>();

	auto ekey = static_cast<sf::Keyboard::Key>(key);
	return sf::Keyboard::isKeyPressed(ekey) && window->IsActive();
}

bool keyup(int key)
{
	auto window = BabyDI::Get<render::Window>();

	auto ekey = static_cast<sf::Keyboard::Key>(key);
	return !sf::Keyboard::isKeyPressed(ekey) && window->IsActive();
}

void logF(const char* message)
{
	log::PrintLine(":: [SCRIPT] {}", message);
}

/*
void playSound(const std::string& sound)
{

}

void playSound(const std::string& sound, const Vector2df& position)
{

}
*/

} // end namespace tdrp
