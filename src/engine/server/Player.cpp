#include "engine/server/Player.h"

#include "engine/server/Server.h"
#include "engine/network/Packet.h"
#include "engine/network/packets/SceneObjectControl.pb.h"
#include "engine/network/construct/SceneObject.h"

namespace tdrp::server
{

void Player::BindServer(server::Server* server)
{
	m_server = server;
}

std::weak_ptr<scene::Scene> Player::SwitchScene(std::shared_ptr<scene::Scene>& new_scene)
{
	auto old_scene = m_current_scene;

	if (auto current_sceneobject = m_current_sceneobject.lock())
	{
		// If this isn't a global scene object, we lose access to it in the new scene.
		if (!current_sceneobject->IsGlobal())
			m_current_sceneobject.reset();
	}

	m_current_scene = new_scene;

	FollowedSceneObjects.clear();

	return old_scene;
}

std::weak_ptr<SceneObject> Player::SwitchControlledSceneObject(std::shared_ptr<SceneObject>& new_scene_object)
{
	auto old_so = m_current_sceneobject;

	m_current_sceneobject = new_scene_object;
	OnSwitchedControl.RunAll(new_scene_object, old_so);

	// If we are a guest, inform the host that we have changed our controlled scene object.
	if (m_server->IsGuest())
	{
		packet::SceneObjectControl packet;
		packet.set_player_id(m_player_id);
		packet.set_sceneobject_id(new_scene_object->ID);
		m_server->Send(network::HOSTID, network::PACKETID(network::Packets::SCENEOBJECTCONTROL), network::Channel::RELIABLE, packet);
	}

	log::PrintLine(":: Player {} taking control of scene object {}.", m_player_id, new_scene_object->ID);

	return old_so;
}

} // end namespace tdrp::server
