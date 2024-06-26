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
		  { "Unknown", sf::Keyboard::Unknown }
		, { "A", sf::Keyboard::A }
		, { "B", sf::Keyboard::B }
		, { "C", sf::Keyboard::C }
		, { "D", sf::Keyboard::D }
		, { "E", sf::Keyboard::E }
		, { "F", sf::Keyboard::F }
		, { "G", sf::Keyboard::G }
		, { "H", sf::Keyboard::H }
		, { "I", sf::Keyboard::I }
		, { "J", sf::Keyboard::J }
		, { "K", sf::Keyboard::K }
		, { "L", sf::Keyboard::L }
		, { "M", sf::Keyboard::M }
		, { "N", sf::Keyboard::N }
		, { "O", sf::Keyboard::O }
		, { "P", sf::Keyboard::P }
		, { "Q", sf::Keyboard::Q }
		, { "R", sf::Keyboard::R }
		, { "S", sf::Keyboard::S }
		, { "T", sf::Keyboard::T }
		, { "U", sf::Keyboard::U }
		, { "V", sf::Keyboard::V }
		, { "W", sf::Keyboard::W }
		, { "X", sf::Keyboard::X }
		, { "Y", sf::Keyboard::Y }
		, { "Z", sf::Keyboard::Z }
		, { "Num0", sf::Keyboard::Num0 }
		, { "Num1", sf::Keyboard::Num1 }
		, { "Num2", sf::Keyboard::Num2 }
		, { "Num3", sf::Keyboard::Num3 }
		, { "Num4", sf::Keyboard::Num4 }
		, { "Num5", sf::Keyboard::Num5 }
		, { "Num6", sf::Keyboard::Num6 }
		, { "Num7", sf::Keyboard::Num7 }
		, { "Num8", sf::Keyboard::Num8 }
		, { "Num9", sf::Keyboard::Num9 }
		, { "Escape", sf::Keyboard::Escape }
		, { "LControl", sf::Keyboard::LControl }
		, { "LShift", sf::Keyboard::LShift }
		, { "LAlt", sf::Keyboard::LAlt }
		, { "LSystem", sf::Keyboard::LSystem }
		, { "RControl", sf::Keyboard::RControl }
		, { "RShift", sf::Keyboard::RShift }
		, { "RAlt", sf::Keyboard::RAlt }
		, { "RSystem", sf::Keyboard::RSystem }
		, { "Menu", sf::Keyboard::Menu }
		, { "LBracket", sf::Keyboard::LBracket }
		, { "RBracket", sf::Keyboard::RBracket }
		, { "Semicolon", sf::Keyboard::Semicolon }
		, { "Comma", sf::Keyboard::Comma }
		, { "Period", sf::Keyboard::Period }
		, { "Quote", sf::Keyboard::Quote }
		, { "Slash", sf::Keyboard::Slash }
		, { "Backslash", sf::Keyboard::Backslash }
		, { "Tilde", sf::Keyboard::Tilde }
		, { "Equal", sf::Keyboard::Equal }
		, { "Hyphen", sf::Keyboard::Hyphen }
		, { "Space", sf::Keyboard::Space }
		, { "Enter", sf::Keyboard::Enter }
		, { "Backspace", sf::Keyboard::Backspace }
		, { "Tab", sf::Keyboard::Tab }
		, { "PageUp", sf::Keyboard::PageUp }
		, { "PageDown", sf::Keyboard::PageDown }
		, { "End", sf::Keyboard::End }
		, { "Home", sf::Keyboard::Home }
		, { "Insert", sf::Keyboard::Insert }
		, { "Delete", sf::Keyboard::Delete }
		, { "Add", sf::Keyboard::Add }
		, { "Subtract", sf::Keyboard::Subtract }
		, { "Multiply", sf::Keyboard::Multiply }
		, { "Divide", sf::Keyboard::Divide }
		, { "Left", sf::Keyboard::Left }
		, { "Right", sf::Keyboard::Right }
		, { "Up", sf::Keyboard::Up }
		, { "Down", sf::Keyboard::Down }
		, { "Numpad0", sf::Keyboard::Numpad0 }
		, { "Numpad1", sf::Keyboard::Numpad1 }
		, { "Numpad2", sf::Keyboard::Numpad2 }
		, { "Numpad3", sf::Keyboard::Numpad3 }
		, { "Numpad4", sf::Keyboard::Numpad4 }
		, { "Numpad5", sf::Keyboard::Numpad5 }
		, { "Numpad6", sf::Keyboard::Numpad6 }
		, { "Numpad7", sf::Keyboard::Numpad7 }
		, { "Numpad8", sf::Keyboard::Numpad8 }
		, { "Numpad9", sf::Keyboard::Numpad9 }
		, { "F1", sf::Keyboard::F1 }
		, { "F2", sf::Keyboard::F2 }
		, { "F3", sf::Keyboard::F3 }
		, { "F4", sf::Keyboard::F4 }
		, { "F5", sf::Keyboard::F5 }
		, { "F6", sf::Keyboard::F6 }
		, { "F7", sf::Keyboard::F7 }
		, { "F8", sf::Keyboard::F8 }
		, { "F9", sf::Keyboard::F9 }
		, { "F10", sf::Keyboard::F10 }
		, { "F11", sf::Keyboard::F11 }
		, { "F12", sf::Keyboard::F12 }
		, { "F13", sf::Keyboard::F13 }
		, { "F14", sf::Keyboard::F14 }
		, { "F15", sf::Keyboard::F15 }
		, { "Pause", sf::Keyboard::Pause }
	});

	lua.new_enum<sf::Mouse::Wheel>("MouseWheel", {
		  { "Vertical", sf::Mouse::VerticalWheel }
		, { "Horizontal", sf::Mouse::HorizontalWheel }
	});

	lua.new_enum<sf::Mouse::Button>("MouseButton", {
		  { "Left", sf::Mouse::Left }
		, { "Right", sf::Mouse::Right }
		, { "Middle", sf::Mouse::Middle }
		, { "XButton1", sf::Mouse::XButton1 }
		, { "XButton2", sf::Mouse::XButton2 }
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
	ObjectAttributes* GetFlags(Game& self)
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
		"OnSceneSwitch", sol::writeonly_property(&Game::SetOnSceneSwitch)
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
