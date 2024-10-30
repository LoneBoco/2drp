#pragma once

#include <chrono>

#include "engine/scene/SceneObject.h"
#include "engine/network/construct/Attributes.h"
#include "engine/network/packets/SceneObjectNew.pb.h"
#include "engine/network/packets/SceneObjectChange.pb.h"
#include "engine/network/packets/SceneObjectShapes.pb.h"

#include <PlayRho/PlayRho.hpp>


namespace tdrp::network
{

inline void readCollisionShapesFromPacket(SceneObjectPtr sceneobject, const packet::SceneObjectShapes& packet, bool is_owner = true)
{
	if (packet.has_body())
	{
		const auto& body = packet.body();

		physics::BodyConfiguration config{};
		config.Type = static_cast<physics::BodyTypes>(static_cast<int>(body.type()));
		config.BodyConf.fixedRotation = body.fixed_rotation();

		for (int i = 0; i < packet.collision_shapes_size(); ++i)
		{
			const auto& shape = packet.collision_shapes(i);
			const auto shapetype = shape.type();
			const auto radius = shape.radius();

			if (shapetype == tdrp::packet::ShapeType::CIRCLE && shape.points_size() > 0)
			{
				const auto& point = shape.points(0);

				playrho::d2::DiskShapeConf conf{};
				conf.UseRadius(radius);
				conf.UseLocation({ point.x(), point.y() });
				conf.UseDensity(1010.0f);

				config.Shapes.emplace_back(playrho::d2::Shape{ conf });
			}
			else if (shapetype == packet::ShapeType::POLYGON && shape.points_size() > 2)
			{
				playrho::d2::PolygonShapeConf conf{};
				playrho::d2::VertexSet set{};
				for (int j = 0; j < shape.points_size(); ++j)
				{
					const auto& point = shape.points(j);
					set.add({ point.x(), point.y() });
				}
				conf.Set(set);

				config.Shapes.emplace_back(playrho::d2::Shape{ conf });
			}
		}

		// If this is a TMX scene object, read the chunk so we know where to set the physics data.
		if (sceneobject->GetType() == SceneObjectType::TMX)
		{
			if (auto tmx = std::dynamic_pointer_cast<TMXSceneObject>(sceneobject); tmx != nullptr && packet.has_chunk_index())
			{
				auto chunk_index = packet.chunk_index();
				tmx->SetChunkCollision(chunk_index, config);
			}
		}
		// Otherwise, set on the scene object itself.
		else
		{
			sceneobject->SetPhysicsBody(config);
		}
	}
}

inline packet::SceneObjectShapes constructCollisionShapesPacket(SceneObjectPtr sceneobject, const physics::BodyConfiguration& config, std::optional<size_t> chunk_index = std::nullopt)
{
	packet::SceneObjectShapes packet{};
	packet.set_id(sceneobject->ID);

	auto* pbody = packet.mutable_body();
	pbody->set_type(static_cast<packet::BodyType>(static_cast<int>(config.Type)));
	pbody->set_fixed_rotation(config.BodyConf.fixedRotation);

	// If the chunk index was specified, write it.
	if (chunk_index.has_value())
	{
		packet.set_chunk_index(static_cast<uint32_t>(chunk_index.value()));
	}

	for (const playrho::d2::Shape& shape : config.Shapes)
	{
		auto* pshape = packet.add_collision_shapes();

		auto shape_type = playrho::d2::GetType(shape);
		if (shape_type == playrho::GetTypeID<playrho::d2::DiskShapeConf>())
		{
			const auto& disk = playrho::d2::TypeCast<const playrho::d2::DiskShapeConf>(&shape);
			auto loc = disk->GetLocation();

			pshape->set_type(packet::ShapeType::CIRCLE);
			pshape->set_radius(disk->GetRadius());

			auto* point = pshape->add_points();
			point->set_x(loc[0]);
			point->set_y(loc[1]);
		}
		else if (shape_type == playrho::GetTypeID<playrho::d2::PolygonShapeConf>())
		{
			const auto& polygon = playrho::d2::TypeCast<const playrho::d2::PolygonShapeConf>(&shape);
			pshape->set_type(packet::ShapeType::POLYGON);

			for (auto i = 0; i < polygon->GetVertexCount(); ++i)
			{
				auto* point = pshape->add_points();
				auto vertex = polygon->GetVertex(i);
				point->set_x(vertex[0]);
				point->set_y(vertex[1]);
			}
		}
	}

	return packet;
}

inline packet::SceneObjectNew constructSceneObjectPacket(SceneObjectPtr sceneobject)
{
	packet::SceneObjectNew object;
	object.set_id(sceneobject->ID);
	object.set_type(static_cast<google::protobuf::uint32>(sceneobject->GetType()));
	object.set_class_(sceneobject->GetClass()->GetName());
	object.set_replicatechanges(sceneobject->ReplicateChanges);
	object.set_ignoresevents(sceneobject->IgnoresEvents);
	object.set_attached_to(sceneobject->GetAttachedId());

	auto scenep = sceneobject->GetCurrentScene().lock();
	if (scenep)
	{
		object.set_scene(scenep->GetName());
	}

	assignAllAttributesToPacket(object, sceneobject->Attributes, &packet::SceneObjectNew::add_attributes);
	assignAllAttributesToPacket(object, sceneobject->Properties, &packet::SceneObjectNew::add_properties);

	// Add TMX tilesets.
	if (sceneobject->GetType() == SceneObjectType::TMX)
	{
		auto tmx = std::dynamic_pointer_cast<TMXSceneObject>(sceneobject);
		if (tmx && !tmx->Tilesets.empty())
		{
			for (const auto& [gid, tileset] : tmx->Tilesets)
			{
				auto* pgids = object.add_tileset_gids();
				pgids->set_name(tileset->File);
				pgids->set_first_gid(gid.FirstGID);
				pgids->set_last_gid(gid.LastGID);
			}
		}
	}

	return object;
}

} // end namespace tdrp::network
