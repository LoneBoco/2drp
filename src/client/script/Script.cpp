#include <SFML/Window.hpp>

#include "client/game/Game.h"
#include "client/script/Script.h"

#include "engine/scene/SceneObject.h"
#include "engine/network/Packet.h"
#include "engine/network/PacketsClient.h"

using tdrp::network::PACKETID;
using tdrp::network::ClientPackets;

namespace tdrp
{

bool keydown(int key);
bool keyup(int key);
void log(const char* message);
int send_event(std::shared_ptr<SceneObject> sender, const char* name, const char* data, Vector2df origin, float radius);

void bind_game(sol::state& lua)
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

    auto game = lua["Game"].get_or_create<sol::table>();
    game.set_function("keydown", &keydown);
    game.set_function("keyup", &keyup);
    game.set_function("log", &log);
}

///////////////////////////////////////////////////////////////////////////////

bool keydown(int key)
{
    auto ekey = static_cast<sf::Keyboard::Key>(key);
    return sf::Keyboard::isKeyPressed(ekey);
}

bool keyup(int key)
{
    auto ekey = static_cast<sf::Keyboard::Key>(key);
    return !sf::Keyboard::isKeyPressed(ekey);
}

void log(const char* message)
{
    std::cout << message << std::endl;
}

int send_event(std::shared_ptr<SceneObject> sender, const char* name, const char* data, Vector2df origin, float radius)
{
    tdrp::packet::CSendEvent packet;
    packet.set_sender(sender->ID);
    packet.set_name(name);
    packet.set_data(data);
    packet.set_x(origin.x);
    packet.set_y(origin.y);
    packet.set_radius(radius);

    BabyDI::Injected<tdrp::Game> game;
    game->Server.Network.Send(0, PACKETID(ClientPackets::SENDEVENT), tdrp::network::Channel::RELIABLE, packet);

    return 0;
}

} // end namespace tdrp
