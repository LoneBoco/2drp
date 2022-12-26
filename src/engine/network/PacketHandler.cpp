#include "engine/network/PacketHandler.h"

#include "engine/server/Server.h"
#include "engine/server/Player.h"

#include "engine/network/Packet.h"
#include "engine/network/PacketsInc.h"
#include "engine/network/construct/SceneObject.h"

#include "engine/filesystem/File.h"
#include "engine/filesystem/Log.h"

using tdrp::server::Server;
using tdrp::server::Player;
using tdrp::network::Packets;
using tdrp::network::construct;

namespace tdrp::network::handlers
{

void handle_ready(Server& server, const uint16_t id);

void handle(Server& server, const uint16_t id, const packet::Error& packet);
void handle(Server& server, const uint16_t id, const packet::Login& packet);
void handle(Server& server, const uint16_t id, const packet::LoginStatus& packet);
void handle(Server& server, const uint16_t id, const packet::ServerInfo& packet);
void handle(Server& server, const uint16_t id, const packet::FileRequest& packet);
void handle(Server& server, const uint16_t id, const packet::FileTransfer& packet);
void handle(Server& server, const uint16_t id, const packet::SwitchScene& packet);
void handle(Server& server, const uint16_t id, const packet::ClientControlScript& packet);
void handle(Server& server, const uint16_t id, const packet::ClientScriptAdd& packet);
void handle(Server& server, const uint16_t id, const packet::ClientScriptDelete& packet);
void handle(Server& server, const uint16_t id, const packet::ClassAdd& packet);
void handle(Server& server, const uint16_t id, const packet::ClassDelete& packet);
void handle(Server& server, const uint16_t id, const packet::SceneObjectNew& packet);
void handle(Server& server, const uint16_t id, const packet::SceneObjectChange& packet);
void handle(Server& server, const uint16_t id, const packet::SceneObjectDelete& packet);
void handle(Server& server, const uint16_t id, const packet::SceneObjectOwnership& packet);
void handle(Server& server, const uint16_t id, const packet::SceneObjectControl& packet);
void handle(Server& server, const uint16_t id, const packet::SceneObjectUnfollow& packet);
void handle(Server& server, const uint16_t id, const packet::SendEvent& packet);
void handle(Server& server, const uint16_t id, const packet::FlagSet& packet);

/////////////////////////////

#define HANDLE(cl) handle(server, id, construct< cl >(packet_data, packet_length))

void network_receive_server(Server& server, const uint16_t id, const uint16_t packet_id, const uint8_t* const packet_data, const size_t packet_length)
{
	// Grab our packet id.
	Packets packet = static_cast<Packets>(packet_id);
	switch (packet)
	{
	case Packets::CLIENTREADY:
		handle_ready(server, id);
		break;
	case Packets::ERROR:
		HANDLE(packet::Error);
		break;
	case Packets::LOGIN:
		HANDLE(packet::Login);
		break;
	case Packets::LOGINSTATUS:
		HANDLE(packet::LoginStatus);
		break;
	case Packets::SERVERINFO:
		HANDLE(packet::ServerInfo);
		break;
	case Packets::FILEREQUEST:
		HANDLE(packet::FileRequest);
		break;
	case Packets::FILETRANSFER:
		HANDLE(packet::FileTransfer);
		break;
	case Packets::SWITCHSCENE:
		HANDLE(packet::SwitchScene);
		break;
	case Packets::CLIENTCONTROLSCRIPT:
		HANDLE(packet::ClientControlScript);
		break;
	case Packets::CLIENTSCRIPTADD:
		HANDLE(packet::ClientScriptAdd);
		break;
	case Packets::CLIENTSCRIPTDELETE:
		HANDLE(packet::ClientScriptDelete);
		break;
	case Packets::CLASSADD:
		HANDLE(packet::ClassAdd);
		break;
	case Packets::SCENEOBJECTNEW:
		HANDLE(packet::SceneObjectNew);
		break;
	case Packets::SCENEOBJECTCHANGE:
		HANDLE(packet::SceneObjectChange);
		break;
	case Packets::SCENEOBJECTDELETE:
		HANDLE(packet::SceneObjectDelete);
		break;
	case Packets::SCENEOBJECTOWNERSHIP:
		HANDLE(packet::SceneObjectOwnership);
		break;
	case Packets::SCENEOBJECTCONTROL:
		HANDLE(packet::SceneObjectControl);
		break;
	case Packets::SCENEOBJECTUNFOLLOW:
		HANDLE(packet::SceneObjectUnfollow);
		break;
	case Packets::SENDEVENT:
		HANDLE(packet::SendEvent);
		break;
	case Packets::FLAGSET:
		HANDLE(packet::FlagSet);
		break;
	}
}

#undef HANDLE

/////////////////////////////

void handle_ready(Server& server, const uint16_t id)
{
	auto player = server.GetPlayerById(id);

	// Switch to the starting scene.
	auto scene = server.GetScene(server.GetPackage()->GetStartingScene());
	player->SwitchScene(scene);

	// Call the OnPlayerJoin script function.
	server.OnPlayerJoin.RunAll(player);
}

/////////////////////////////

void handle(Server& server, const uint16_t id, const packet::Error& packet)
{
	auto& error = packet.message();
	log::PrintLine("Error: {}", error);
}

void handle(Server& server, const uint16_t id, const packet::Login& packet)
{
	assert(false);
}

void handle(Server& server, const uint16_t id, const packet::LoginStatus& packet)
{
	const auto success = packet.success();
	const auto& msg = packet.message();
	const auto player_id = packet.player_id();

	if (success)
	{
		log::PrintLine("<- Login successful.");
		server.SetPlayerNumber(player_id);
	}
	else
	{
		log::PrintLine("<- Login failed: {}", msg);
		// TODO: Disconnect.
	}
}

void handle(Server& server, const uint16_t id, const packet::ServerInfo& packet)
{
	const auto& uniqueid = packet.uniqueid();
	const auto& package = packet.package();
	const auto& loadingscene = packet.loadingscene();

	// Try to bind the package directory, if it exists.
	server.FileSystem.Bind(filesystem::path("packages") / package);

	if (server.IsSinglePlayer())
		return;

	// If we are a client, bind our downloads directory.
	if (!server.IsHost())
	{
		server.DefaultDownloadPath = filesystem::path("downloads") / uniqueid;
		server.FileSystem.Bind(server.DefaultDownloadPath);
	}

	// Rage.
	auto packet_copy = std::make_shared<packet::ServerInfo>();
	packet_copy->CopyFrom(packet);

	std::thread process_missing_files(
		[=, &server]()
		{
			// Blocks thread.
			server.FileSystem.WaitUntilFilesSearched();

			std::list<std::string> download_list;
			for (int i = 0; i < packet_copy->files_size(); ++i)
			{
				const auto& file_entry = packet_copy->files(i);

				const auto& name = file_entry.name();
				const auto& size = file_entry.size();
				const auto& date = file_entry.date();
				const auto& crc32 = file_entry.crc32();

				// Check to see if we have this file.
				bool request_file = true;
				if (server.FileSystem.HasFile(name))
				{
					auto data = server.FileSystem.GetFileData(name);
					if (data.FileSize == size && data.TimeSinceEpoch == date && data.CRC32 == crc32)
						request_file = false;
				}

				if (request_file)
					download_list.push_back(name);
			}
			if (!download_list.empty())
				server.DownloadManager.AddToQueue(std::move(download_list));
			else server.DownloadManager.TryClearProcessingFlag();
		}
	);
	process_missing_files.detach();
}

void handle(Server& server, const uint16_t id, const packet::FileRequest& packet)
{
	if (server.IsSinglePlayer())
		return;

	for (int i = 0; i < packet.file_size(); ++i)
	{
		filesystem::path file_path = packet.file(i);

		// TODO: Better file sending with a class to send data in proper chunks.

		auto file = server.FileSystem.GetFile(file_path.filename());
		if (file != nullptr)
		{
			tdrp::packet::FileTransfer message;
			message.set_type(tdrp::packet::FileTransfer_Type_PACKAGE);
			message.set_name(file_path.filename().string());
			message.set_date(static_cast<google::protobuf::int64>(file->ModifiedTime()));
			message.set_file(file->ReadAsString());

			server.GetNetwork().Send(id, network::PACKETID(Packets::FILETRANSFER), network::Channel::FILE, message);
		}
	}
}

void handle(Server& server, const uint16_t id, const packet::FileTransfer& packet)
{
	if (server.IsSinglePlayer())
		return;

	const auto& name = packet.name();
	const auto& date = packet.date();
	const auto& file = packet.file();

	if (server.DefaultDownloadPath.empty())
	{
		log::PrintLine("** [ERROR] Received TransferFile but no default download path set.");
		return;
	}

	// Write the file.
	filesystem::path file_location = server.DefaultDownloadPath / name;
	std::ofstream new_file(file_location.string(), std::ios::binary | std::ios::trunc);
	new_file.write(file.c_str(), file.size());
	new_file.close();

	// Set the last write time.
	// TODO: Improve portability with C++20.
	filesystem::last_write_time(file_location, filesystem::file_time_type(filesystem::file_time_type::duration(date)));

	server.DownloadManager.InformComplete(name);
}

void handle(Server& server, const uint16_t id, const packet::SwitchScene& packet)
{
	// Client handles.
	//assert(false);
}

void handle(Server& server, const uint16_t id, const packet::ClientControlScript& packet)
{
	// Client handles.
	//assert(false);
}

void handle(Server& server, const uint16_t id, const packet::ClientScriptAdd& packet)
{
	// Client handles.
	//assert(false);
}

void handle(Server& server, const uint16_t id, const packet::ClientScriptDelete& packet)
{
	// Client handles.
	//assert(false);
}

void handle(Server& server, const uint16_t id, const packet::ClassAdd& packet)
{
	const auto& name = packet.name();
	const auto& script_client = packet.scriptclient();
	const auto& script_server = packet.scriptserver();

	log::PrintLine(":: Adding class {}.", name);

	auto c = server.GetObjectClass(name);
	c->ScriptClient = script_client;
	c->ScriptServer = script_server;
	c->Attributes.GetMap().clear();

	for (int i = 0; i < packet.attributes_size(); ++i)
	{
		const auto& attr = packet.attributes(i);
		const auto& id = attr.id();
		const auto& name = attr.name();
		const auto& replicated = attr.replicated();
		const auto& update_rate = attr.update_rate();

		AttributePtr attribute;
		switch (attr.value_case())
		{
		case packet::ClassAdd_Attribute::ValueCase::kAsInt:
			attribute = c->Attributes.AddAttribute(name, attr.as_int(), static_cast<uint16_t>(id));
			break;
		case packet::ClassAdd_Attribute::ValueCase::kAsDouble:
			attribute = c->Attributes.AddAttribute(name, attr.as_double(), static_cast<uint16_t>(id));
			break;
		case packet::ClassAdd_Attribute::ValueCase::kAsString:
			attribute = c->Attributes.AddAttribute(name, attr.as_string(), static_cast<uint16_t>(id));
			break;
		}

		attribute->Replicated = replicated;
		attribute->NetworkUpdate.UpdateRate = std::chrono::milliseconds{ update_rate };
	}
}

void handle(Server& server, const uint16_t id, const packet::ClassDelete& packet)
{
	const auto& name = packet.name();

	log::PrintLine("!! Deleting class {}", name);

	server.DeleteObjectClass(name);
}

void handle(Server& server, const uint16_t id, const packet::SceneObjectNew& packet)
{
	const auto pid = packet.id();
	const auto ptype = packet.type();
	const auto& pclass = packet.class_();
	const auto& pscript = packet.script();
	const auto pattached_to = packet.attached_to();
	const auto& pscene = packet.scene();

	auto class_ = server.GetObjectClass(pclass);
	auto type = static_cast<SceneObjectType>(ptype);

	auto scene = server.GetScene(pscene);
	if (!scene)
	{
		log::PrintLine("** ERROR: Attempted to add scene object to scene \"{}\" but the scene does not exist.", pscene);
		return;
	}

	std::shared_ptr<SceneObject> so = server.GetSceneObjectById(pid);
	if (so != nullptr)
	{
		log::PrintLine("!! Scene object {} ({}) already exists, skipping add.", pid, pclass);
	}
	else
	{
		log::PrintLine(":: Adding scene object {} ({}).", pid, pclass);

		// Create our new scene object.
		switch (type)
		{
		case SceneObjectType::STATIC:
			so = std::make_shared<StaticSceneObject>(class_, pid);
			break;
		case SceneObjectType::ANIMATED:
			so = std::make_shared<AnimatedSceneObject>(class_, pid);
			break;
		case SceneObjectType::TILEMAP:
			so = std::make_shared<TiledSceneObject>(class_, pid);
			break;
		case SceneObjectType::TMX:
			so = std::make_shared<TMXSceneObject>(class_, pid);
			break;
		case SceneObjectType::TEXT:
			so = std::make_shared<TextSceneObject>(class_, pid);
			break;
		default:
			so = std::make_shared<SceneObject>(class_, pid);
			break;
		}

		// Set attached to.
		// Do this before we read in attributes/props as attaching sets the position to {0, 0}.
		if (pattached_to != 0)
		{
			auto attached = server.GetSceneObjectById(pattached_to);
			if (attached)
				so->AttachTo(attached);
		}

		// Add props and attributes.
		for (int i = 0; i < packet.attributes_size(); ++i)
		{
			const auto& attribute = packet.attributes(i);
			AttributePtr attr;

			switch (attribute.value_case())
			{
			case packet::SceneObjectNew_Attribute::kAsInt:
				attr = so->Attributes.AddAttribute(attribute.name(), attribute.as_int(), attribute.id());
				break;
			case packet::SceneObjectNew_Attribute::kAsDouble:
				attr = so->Attributes.AddAttribute(attribute.name(), attribute.as_double(), attribute.id());
				break;
			case packet::SceneObjectNew_Attribute::kAsString:
				attr = so->Attributes.AddAttribute(attribute.name(), attribute.as_string(), attribute.id());
				break;
			}

			attr->Replicated = attribute.replicated();
			attr->NetworkUpdate.UpdateRate = std::chrono::milliseconds{ attribute.update_rate() };
		}

		for (int i = 0; i < packet.properties_size(); ++i)
		{
			const auto& prop = packet.properties(i);
			auto soprop = so->Properties.Get(prop.name());
			if (soprop)
			{
				switch (prop.value_case())
				{
				case packet::SceneObjectNew_Attribute::kAsInt:
					soprop->Set(prop.as_int());
					break;
				case packet::SceneObjectNew_Attribute::kAsDouble:
					soprop->Set(prop.as_double());
					break;
				case packet::SceneObjectNew_Attribute::kAsString:
					soprop->Set(prop.as_string());
					break;
				}

				soprop->Replicated = prop.replicated();
				soprop->NetworkUpdate.UpdateRate = std::chrono::milliseconds{ prop.update_rate() };
			}
		}

		// Add it to the appropriate scene.
		scene->AddObject(so);
	}

	// Handle the script.
	if (so)
	{
		if (class_)
		{
			server.Script->RunScript("sceneobject_cl_" + std::to_string(pid) + "_c_" + pclass, class_->ScriptClient, so);
		}

		so->ClientScript = pscript;
		server.Script->RunScript("sceneobject_cl_" + std::to_string(pid), so->ClientScript, so);

		so->OnCreated.RunAll();
	}
}

void handle(Server& server, const uint16_t id, const packet::SceneObjectChange& packet)
{
	const auto pid = packet.id();
	const auto pattached_to = packet.attached_to();

	auto so = server.GetSceneObjectById(pid);
	if (!so) return;

	// If we own this scene object, don't allow changes.
	if (auto op = so->GetOwningPlayer().lock(); op == server.GetPlayer())
		return;

	// Set attached to.
	// Do this before we read in attributes/props as attaching sets the position to {0, 0}.
	if (pattached_to != 0)
	{
		auto attached = server.GetSceneObjectById(pattached_to);
		if (attached)
			so->AttachTo(attached);
	}

	for (int i = 0; i < packet.attributes_size(); ++i)
	{
		const auto& attribute = packet.attributes(i);
		const auto attribute_id = static_cast<uint16_t>(attribute.id());
		auto soattrib = so->Attributes.Get(attribute_id);
		if (!soattrib)
		{
			const auto& name = attribute.name();
			soattrib = so->Attributes.GetOrCreate(name);
		}
		if (!soattrib) continue;

		switch (attribute.value_case())
		{
		case packet::SceneObjectChange_Attribute::kAsInt:
			soattrib->Set(attribute.as_int());
			break;
		case packet::SceneObjectChange_Attribute::kAsDouble:
			soattrib->Set(attribute.as_double());
			break;
		case packet::SceneObjectChange_Attribute::kAsString:
			soattrib->Set(attribute.as_string());
			break;
		}

		soattrib->Replicated = attribute.replicated();
		soattrib->NetworkUpdate.UpdateRate = std::chrono::milliseconds{ attribute.update_rate() };

		// Force dirty flag.
		soattrib->SetDirty(true, AttributeDirty::CLIENT);
	}

	for (int i = 0; i < packet.properties_size(); ++i)
	{
		const auto& prop = packet.properties(i);
		const auto prop_id = static_cast<uint16_t>(prop.id());
		auto soprop = so->Properties.Get(PropertyById(prop_id));
		if (!soprop)
			continue;

		switch (prop.value_case())
		{
		case packet::SceneObjectChange_Attribute::kAsInt:
			soprop->Set(prop.as_int());
			break;
		case packet::SceneObjectChange_Attribute::kAsDouble:
			soprop->Set(prop.as_double());
			break;
		case packet::SceneObjectChange_Attribute::kAsString:
			soprop->Set(prop.as_string());
			break;
		}

		soprop->Replicated = prop.replicated();
		soprop->NetworkUpdate.UpdateRate = std::chrono::milliseconds{ prop.update_rate() };

		// Force dirty flag.
		// If something came across in this packet, it should force an update.  It might be an animation change.
		// Some animation systems (gani) will restart the animation if the animation prop was set to itself.
		soprop->SetDirty(true, AttributeDirty::CLIENT);
	}

	// Send changes to other players.
	auto scene = so->GetCurrentScene().lock();
	if (scene)
	{
		auto origin = server.GetPlayerById(id);
		server.SendToScene(scene, so->GetPosition(), PACKETID(Packets::SCENEOBJECTCHANGE), network::Channel::RELIABLE, packet, { server.GetPlayer(), origin });
	}
}

void handle(Server& server, const uint16_t id, const packet::SceneObjectDelete& packet)
{
	const auto pid = packet.id();

	log::PrintLine("<- Deleting scene object {}", pid);

	server.DeleteSceneObject(pid);
}

void handle(Server& server, const uint16_t id, const packet::SceneObjectOwnership& packet)
{
	const auto sceneobject_id = packet.sceneobject_id();
	const auto old_player_id = packet.old_player_id();
	const auto new_player_id = packet.new_player_id();

	log::PrintLine("<- SceneObjectOwnership: Player {} takes ownership of {} from player {}.", new_player_id, sceneobject_id, old_player_id);

	auto so = server.GetSceneObjectById(sceneobject_id);
	auto old_player = server.GetPlayerById(old_player_id);
	auto new_player = server.GetPlayerById(new_player_id);

	if (!so)
		return;

	if (!server.IsSinglePlayer())
	{
		so->SetOwningPlayer(new_player);

		if (new_player) new_player->FollowedSceneObjects.insert(sceneobject_id);

		so->OnOwnershipChange.RunAll(new_player, old_player);
	}
}

void handle(Server& server, const uint16_t id, const packet::SceneObjectControl& packet)
{
	const auto& player_id = packet.player_id();
	const auto& sceneobject_id = packet.sceneobject_id();

	auto player = server.GetPlayerById(player_id);
	auto so = server.GetSceneObjectById(sceneobject_id);
	if (!player || !so) return;

	// If the player doesn't own the scene object, don't accept this change.
	if (auto owner = so->GetOwningPlayer().lock(); owner != player)
		return;

	log::PrintLine("<-- SceneObjectControl: Player {} takes control of {}.", player_id, sceneobject_id);

	player->SwitchControlledSceneObject(so);
}

void handle(Server& server, const uint16_t id, const packet::SceneObjectUnfollow& packet)
{
	auto player = server.GetPlayerById(id);

	for (int i = 0; i < packet.id_size(); ++i)
	{
		const auto& oid = packet.id(i);
		player->FollowedSceneObjects.erase(oid);

		log::PrintLine("<- Removing scene object {} from player {}.", oid, id);
	}
}

void handle(Server& server, const uint16_t id, const packet::SendEvent& packet)
{
	const auto& sender = packet.sender();
	const auto& pscene = packet.scene();
	const auto& name = packet.name();
	const auto& data = packet.data();
	const auto& x = packet.x();
	const auto& y = packet.y();
	const auto& radius = packet.radius();

	auto sender_so = server.GetSceneObjectById(sender);
	auto scene = server.GetScene(pscene);
	Vector2df origin{ x, y };

	if (scene)
	{
		log::PrintLine("<- Event \"{}\" from {} to scene \"{}\" at ({}, {})-{} with data: {}", name, sender, pscene, x, y, radius, data);

		// Get objects in range.
		auto targets = scene->FindObjectsBoundInRangeOf(origin, radius);

		// Get hits.
		log::Print("---- hit");
		std::set<SceneObjectPtr> hits;
		for (const auto& target : targets)
		{
			log::Print(", {}", target->ID);
			hits.insert(target);
		}
		log::Print("\n");

		// Run events on each one.
		std::for_each(std::begin(hits), std::end(hits), [&](const auto& target) { target->OnEvent.RunAll(sender_so, name, data, origin, radius); });

		// Run event on the server.
		auto player = server.GetPlayerById(id);
		server.OnEvent.RunAll(sender_so, name, data, origin, radius, player);
	}
	else
	{
		log::PrintLine("<- Received global event \"{}\" from player {} with data: {}", name, id, data);

		// Global events.
		auto player = server.GetPlayerById(id);
		server.OnServerEvent.RunAll(sender_so, name, data, origin, radius, player);
	}
}

void handle(Server& server, const uint16_t id, const packet::FlagSet& packet)
{
	auto player = server.GetPlayerById(id);
	if (player == nullptr) return;
	if (player == server.GetPlayer()) return;

	for (int i = 0; i < packet.attributes_size(); ++i)
	{
		const auto& attribute = packet.attributes(i);
		const auto attribute_id = static_cast<uint16_t>(attribute.id());
		auto flag = player->Account.Flags.Get(attribute_id);
		if (!flag)
		{
			const auto& name = attribute.name();
			flag = player->Account.Flags.GetOrCreate(name);
		}
		if (!flag) continue;

		switch (attribute.value_case())
		{
		case packet::SceneObjectChange_Attribute::kAsInt:
			flag->Set(attribute.as_int());
			break;
		case packet::SceneObjectChange_Attribute::kAsDouble:
			flag->Set(attribute.as_double());
			break;
		case packet::SceneObjectChange_Attribute::kAsString:
			flag->Set(attribute.as_string());
			break;
		}

		flag->Replicated = attribute.replicated();
		flag->NetworkUpdate.UpdateRate = std::chrono::milliseconds{ attribute.update_rate() };

		flag->SetDirty(true, AttributeDirty::CLIENT);
		flag->ProcessDirty(AttributeDirty::CLIENT);
	}
}

} // end namespace tdrp::handlers
