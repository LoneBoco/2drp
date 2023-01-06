#pragma once

#include <chrono>

#include "engine/scene/SceneObject.h"
#include "engine/network/construct/Attributes.h"
#include "engine/network/packets/SceneObjectNew.pb.h"
#include "engine/network/packets/SceneObjectChange.pb.h"

#include <PlayRho/PlayRho.hpp>


namespace tdrp::network
{

template <class T>
inline void readCollisionFromPacket(SceneObjectPtr sceneobject, const T& packet)
{
	auto scene = sceneobject->GetCurrentScene().lock();
	if (!scene) return;

	auto& world = scene->Physics.GetWorld();
	auto ppu = scene->Physics.GetPixelsPerUnit();
	auto body = sceneobject->GetInitializedPhysicsBody();
	if (!body.has_value()) return;

	scene->Physics.AddSceneObject(sceneobject);

	if (packet.has_body())
	{
		const auto& pbody = packet.body();
		const auto bodytype = static_cast<playrho::BodyType>(static_cast<int>(pbody.type()));
		const auto fixedrotation = pbody.fixed_rotation();
		playrho::d2::SetType(world, body.value(), bodytype);
		playrho::d2::SetFixedRotation(world, body.value(), fixedrotation);
	}

	for (int i = 0; i < packet.collision_size(); ++i)
	{
		const auto& shape = packet.collision(i);
		const auto shapetype = shape.type();
		const auto radius = shape.radius();

		if (shapetype == packet::ShapeType::CIRCLE && shape.points_size() > 0)
		{
			const auto& point = shape.points(0);

			playrho::d2::DiskShapeConf conf{};
			conf.UseRadius(radius);
			conf.UseLocation({ point.x(), point.y() });

			auto shapeid = playrho::d2::CreateShape(world, conf);
			playrho::d2::Attach(world, body.value(), shapeid);
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

			auto shapeid = playrho::d2::CreateShape(world, conf);
			playrho::d2::Attach(world, body.value(), shapeid);
		}
	}
}

template <class T>
inline void writeCollisionToPacket(const playrho::BodyID& bodyId, scene::ScenePtr scene, T& packet)
{
	auto& world = scene->Physics.GetWorld();
	const auto& body = world.GetBody(bodyId);

	auto pbody = packet.mutable_body();
	pbody->set_type(static_cast<packet::BodyType>(static_cast<int>(body.GetType())));
	pbody->set_fixed_rotation(body.IsFixedRotation());

	auto shapes = playrho::d2::GetShapes(world, bodyId);
	for (const auto& shapeid : shapes)
	{
		auto pshape = packet.add_collision();

		const auto& shape = playrho::d2::GetShape(world, shapeid);
		auto disk = playrho::d2::TypeCast<const playrho::d2::DiskShapeConf>(&shape);
		auto polygon = playrho::d2::TypeCast<const playrho::d2::PolygonShapeConf>(&shape);
		if (disk != nullptr)
		{
			pshape->set_type(packet::ShapeType::CIRCLE);
			pshape->set_radius(disk->GetRadius());

			auto point = pshape->add_points();
			auto location = disk->GetLocation();
			point->set_x(location[0]);
			point->set_y(location[1]);
		}
		else if (polygon != nullptr)
		{
			pshape->set_type(packet::ShapeType::POLYGON);
			for (auto i = 0; i < polygon->GetVertexCount(); ++i)
			{
				auto point = pshape->add_points();
				auto vertex = polygon->GetVertex(i);
				point->set_x(vertex[0]);
				point->set_y(vertex[1]);
			}
		}
	}
}

inline packet::SceneObjectNew constructSceneObjectPacket(SceneObjectPtr sceneobject)
{
	packet::SceneObjectNew object;
	object.set_id(sceneobject->ID);
	object.set_type(static_cast<google::protobuf::uint32>(sceneobject->GetType()));
	object.set_class_(sceneobject->GetClass()->GetName());
	object.set_non_replicated(sceneobject->NonReplicated);
	object.set_attached_to(sceneobject->GetAttachedId());

	auto scenep = sceneobject->GetCurrentScene().lock();
	if (scenep)
	{
		object.set_scene(scenep->GetName());
	}

	assignAllAttributesToPacket(object, sceneobject->Attributes, &packet::SceneObjectNew::add_attributes);
	assignAllAttributesToPacket(object, sceneobject->Properties, &packet::SceneObjectNew::add_properties);

	if (sceneobject->PhysicsBody.has_value() && scenep)
		writeCollisionToPacket(sceneobject->PhysicsBody.value(), scenep, object);

	return object;
}

} // end namespace tdrp::network
