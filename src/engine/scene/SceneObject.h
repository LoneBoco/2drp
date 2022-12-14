#pragma once

#include <deque>
#include <set>
#include <any>

//#include <Box2D/Box2D.h>
#include <tmxlite/Map.hpp>

#include "engine/common.h"

#include "engine/component/Component.h"
#include "engine/scene/ObjectClass.h"
#include "engine/scene/ObjectAttributes.h"
#include "engine/scene/ObjectProperties.h"
#include "engine/scene/Tileset.h"
#include "engine/script/Function.h"


namespace tdrp::server
{
	class Player;
}
namespace tdrp::scene
{
	class Scene;
}

namespace tdrp
{

//! The type of a scene object.
enum class SceneObjectType
{
	DEFAULT = 0,
	STATIC,
	ANIMATED,
	TILEMAP,
	TMX,
	TEXT,

	COUNT
};

using SceneObjectID = uint32_t;

/*
namespace physics
{

	//! Type of physics object.
	enum EBodyType
	{
		EBT_STATIC,
		EBT_KINETIC,
		EBT_DYNAMIC,
		EBT_COUNT
	};

	//! Physics struct.
	class Physics
	{
		public:
			Physics() : Body(nullptr), Owner(false) {}
			~Physics() {}

			b2Body* Body;
			bool Owner;

			EBodyType get_type() const
			{
				switch (Body->GetType())
				{
					default:
					case b2_staticBody:
						return EBT_STATIC;
					case b2_kinematicBody:
						return EBT_KINETIC;
					case b2_dynamicBody:
						return EBT_DYNAMIC;
				}
			}

			void set_type(EBodyType type)
			{
				switch (type)
				{
					default:
					case EBT_STATIC:
						Body->SetType(b2_staticBody);
						break;
					case EBT_KINETIC:
						Body->SetType(b2_kinematicBody);
						break;
					case EBT_DYNAMIC:
						Body->SetType(b2_dynamicBody);
						break;
				}
			}

			void add_collision_circle(float radius, const core::vector2df& center, float density = 1.0f, float friction = 0.2f, float restitution = 0.0f);
			void add_collision_box(const core::vector2df& dimension, const core::vector2df& center, float angle = 0.0f, float density = 1.0f, float friction = 0.2f, float restitution = 0.0f);
			void add_collision_poly(const std::vector<core::vector2df>& vertices, float density = 1.0f, float friction = 0.2f, float restitution = 0.0f);
			void add_collision_poly(const std::vector<b2Vec2>& vertices, float density = 1.0f, float friction = 0.2f, float restitution = 0.0f);
			void add_collision_edge(const core::vector2df& v1, const core::vector2df& v2, bool hasVertex0 = false, const core::vector2df& v0 = core::vector2df(), bool hasVertex3 = false, const core::vector2df& v3 = core::vector2df(), float density = 1.0f, float friction = 0.2f, float restitution = 0.0f);

			void apply_impulse(const core::vector2df& impulse, const core::vector2df& point)
			{
				uint8_t ppu = CPhysicsManager::Instance().get_pixels_per_unit();
				//b2Vec2 p = Body->GetWorldPoint(b2Vec2(point.X, point.Y));
				b2Vec2 p = b2Vec2(point.X / ppu, point.Y / ppu);
				Body->ApplyLinearImpulse(b2Vec2(impulse.X, impulse.Y), p);
			}

			void apply_force(const core::vector2df& force, const core::vector2df& point)
			{
				uint8_t ppu = CPhysicsManager::Instance().get_pixels_per_unit();
				//b2Vec2 p = Body->GetWorldPoint(b2Vec2(point.X, point.Y));
				b2Vec2 p = b2Vec2(point.X / ppu, point.Y / ppu);
				Body->ApplyForce(b2Vec2(force.X, force.Y), p);
			}

			void set_velocity(const core::vector2df& velocity)
			{
				Body->SetLinearVelocity(b2Vec2(velocity.X, velocity.Y));
			}

			core::vector2df get_velocity() const
			{
				b2Vec2 v = Body->GetLinearVelocity();
				return core::vector2df(v.x, v.y);
			}

			void set_rotation(float rotation)
			{
				Body->SetTransform(Body->GetPosition(), rotation * core::DEGTORAD);
			}

			float get_rotation() const
			{
				return Body->GetAngle() * core::RADTODEG;
			}

			void set_linear_dampening(float dampening)
			{
				Body->SetLinearDamping(dampening);
			}

			float get_linear_dampening() const
			{
				return Body->GetLinearDamping();
			}

			void set_angular_dampening(float dampening)
			{
				Body->SetAngularDamping(dampening);
			}

			float get_angular_dampening() const
			{
				return Body->GetAngularDamping();
			}

			void set_fixed_rotation(bool value)
			{
				Body->SetFixedRotation(value);
			}

			bool get_fixed_rotation() const
			{
				return Body->IsFixedRotation();
			}
	};

} // end namespace physics
*/

constexpr SceneObjectID GlobalSceneObjectIDFlag = 0x80'00'00'00;

class SceneObject : public ComponentEntity
{
	SCRIPT_SETUP;
	SCRIPT_FUNCTION(OnCreated);
	SCRIPT_FUNCTION(OnUpdate);
	SCRIPT_FUNCTION(OnEvent);
	SCRIPT_FUNCTION(OnOwnershipChange);
	SCRIPT_FUNCTION(OnCollision);
	SCRIPT_FUNCTION(OnAnimationEnd);
	SCRIPT_FUNCTION(OnAttributeChange);
	SCRIPT_FUNCTION(OnAttached);

public:
	SceneObject(const std::shared_ptr<ObjectClass> c, const SceneObjectID id);
	virtual ~SceneObject();

	SceneObject& operator=(const SceneObject& other);

	SceneObject() = delete;
	SceneObject(const SceneObject& other) = delete;
	SceneObject(SceneObject&& other) = delete;
	SceneObject& operator=(SceneObject&& other) = delete;

public:
	std::weak_ptr<const ObjectClass> GetClass() const;
	std::shared_ptr<ObjectClass> GetClass();
	std::weak_ptr<server::Player> GetOwningPlayer() const;
	std::weak_ptr<scene::Scene> GetCurrentScene() const;

	void SetOwningPlayer(std::shared_ptr<server::Player> player);
	void SetCurrentScene(std::shared_ptr<scene::Scene> scene);

	void AttachTo(std::shared_ptr<SceneObject> other);
	SceneObjectID GetAttachedId();

public:
	virtual SceneObjectType GetType() const
	{
		return SceneObjectType::DEFAULT;
	}

	virtual Vector2df GetPosition() const;
	virtual void SetPosition(const Vector2df& position);

	virtual int64_t GetDepth() const;
	virtual void SetDepth(int64_t z);

	virtual Vector2df GetScale() const;
	virtual void SetScale(const Vector2df& scale);

	// Velocity
	// Force
	// Torque

	virtual float GetRotation() const;
	virtual void SetRotation(float rotation);

	virtual uint64_t GetDirection() const;
	virtual void SetDirection(uint64_t dir);

	virtual std::string GetImage() const;
	virtual void SetImage(const std::string& image);

	virtual std::string GetEntity() const;
	virtual void SetEntity(const std::string& image);

	virtual std::string GetAnimation() const;
	virtual void SetAnimation(const std::string& image);

	virtual std::string GetText() const;
	virtual void SetText(const std::string& text);

	virtual Rectf GetBounds() const;

/*
	//! Sets the callback function used when update() is called.
	//! \param callback The callback function to call.
	void set_update_callback(FSceneObjectUpdate callback)
	{
		UpdateCallback = callback;
	}

	//! Sets the callback function used when update_physics() is called.
	//! \param callback The callback function to call.
	void set_physics_update_callback(FSceneObjectUpdate callback)
	{
		PhysicsUpdateCallback = callback;
	}
*/

	//! Updates the object and clears the list of changed attributes and properties.
	// void Update();

	//! Updates the object after the physics system modifies it.
	// void update_physics();

	//! Clears the scene object of data accumulated this frame.
/*
	void clear_frame_data()
	{
		ChangedAttributes.clear();
		ChangedProperties.clear();
	}
*/
	bool IsGlobal()
	{
		return (ID & GlobalSceneObjectIDFlag) != 0;
	}

	const SceneObjectID ID;
	std::string Name;
	bool Visible = true;
	bool NonReplicated = false;

	ObjectAttributes Attributes;
	ObjectProperties Properties;

	std::string ClientScript;
	std::string ServerScript;

	//! Physics.
	// physics::Physics Physics;

protected:
	const std::shared_ptr<ObjectClass> m_object_class;
	std::weak_ptr<server::Player> m_owning_player;
	std::weak_ptr<scene::Scene> m_current_scene;
	std::weak_ptr<SceneObject> m_attached_to;
	std::vector<std::weak_ptr<SceneObject>> m_attached;
	// FSceneObjectUpdate UpdateCallback;
	// FSceneObjectUpdate PhysicsUpdateCallback;
	// b2Vec2 PreviousPhysicsPosition;
};

using SceneObjectPtr = std::shared_ptr<SceneObject>;

inline std::weak_ptr<const ObjectClass> SceneObject::GetClass() const
{
	return m_object_class;
}
inline std::shared_ptr<ObjectClass> SceneObject::GetClass()
{
	return m_object_class;
}
inline std::weak_ptr<server::Player> SceneObject::GetOwningPlayer() const
{
	return m_owning_player;
}
inline std::weak_ptr<scene::Scene> SceneObject::GetCurrentScene() const
{
	return m_current_scene;
}


/////////////////////////////

class StaticSceneObject : public SceneObject
{
public:
	StaticSceneObject(const std::shared_ptr<ObjectClass> c, const SceneObjectID id) : SceneObject(c, id)	{}
	virtual ~StaticSceneObject() {}

	SceneObjectType GetType() const override
	{
		return SceneObjectType::STATIC;
	}
};

/////////////////////////////

class AnimatedSceneObject : public SceneObject
{
public:
	AnimatedSceneObject(const std::shared_ptr<ObjectClass> c, const SceneObjectID id) : SceneObject(c, id) {}
	virtual ~AnimatedSceneObject() {}

	virtual SceneObjectType GetType() const override
	{
		return SceneObjectType::ANIMATED;
	}

public:
	AnimatedSceneObject& operator=(const AnimatedSceneObject& other);

public:
	void SetAnimation(const std::string& image) override;
};

/////////////////////////////

class TiledSceneObject : public SceneObject
{
public:
	TiledSceneObject(const std::shared_ptr<ObjectClass> c, const SceneObjectID id) : SceneObject(c, id) {}
	virtual ~TiledSceneObject() {}

	virtual SceneObjectType GetType() const override
	{
		return SceneObjectType::TILEMAP;
	}

public:
	virtual Rectf GetBounds() const override
	{
		if (Tileset == nullptr)
			return Rectf();
		return Rectf(GetPosition(), math::convert<float>(Dimension) * math::convert<float>(Tileset->TileDimensions));
	}

public:
	Vector2di Dimension;
	std::vector<char> TileData;
	std::shared_ptr<scene::Tileset> Tileset;
};

/////////////////////////////

class TMXSceneObject : public SceneObject
{
public:
	TMXSceneObject(const std::shared_ptr<ObjectClass> c, const SceneObjectID id) : SceneObject(c, id) {}
	virtual ~TMXSceneObject() {}

	TMXSceneObject& operator=(const TMXSceneObject& other);

	virtual SceneObjectType GetType() const override
	{
		return SceneObjectType::TMX;
	}

public:
	virtual Rectf GetBounds() const override;

public:
	std::shared_ptr<tmx::Map> TmxMap;
	uint8_t Layer = 0;
	Rectf Bounds;
};

/////////////////////////////

class TextSceneObject : public SceneObject
{
public:
	TextSceneObject(const std::shared_ptr<ObjectClass> c, const SceneObjectID id) : SceneObject(c, id), m_font_size(12) {}
	virtual ~TextSceneObject() {}

	SceneObjectType GetType() const override
	{
		return SceneObjectType::TEXT;
	}

public:
	const std::string& GetFont() const;
	void SetFont(const std::string& font);

	uint32_t GetFontSize() const;
	void SetFontSize(uint32_t size);

	bool GetCentered() const;
	void SetCentered(bool centered);

protected:
	std::string m_font;
	uint32_t m_font_size;
	bool m_centered = false;
};

///////////////////////////////////////////////////////////////////////////////

template <typename T>
T getPropsPacket(ObjectAttributes& attributes, auto adder)
{
	T packet;

	// Loop through all dirty attributes and add them to the packet.
	for (auto& attribute : ObjectAttributes::Dirty(attributes))
	{
		attribute.UpdateDispatch.Post(attribute.GetId());
		attributes.DirtyUpdateDispatch.Post(attribute.GetId());

		// If our dispatch removed the dirty flag, skip this attribute.
		if (!attribute.GetIsDirty())
			continue;

		attribute.SetIsDirty(false);

		auto attr = std::invoke(adder, packet);
		attr->set_id(attribute.GetId());
		attr->set_name(attribute.GetName());

		switch (attribute.GetType())
		{
		case AttributeType::SIGNED:
			attr->set_as_int(attribute.GetSigned());
			break;
		case AttributeType::UNSIGNED:
			attr->set_as_uint(attribute.GetUnsigned());
			break;
		case AttributeType::FLOAT:
			attr->set_as_float(attribute.GetFloat());
			break;
		case AttributeType::DOUBLE:
			attr->set_as_double(attribute.GetDouble());
			break;
		default:
		case AttributeType::STRING:
			attr->set_as_string(attribute.GetString());
			break;
		}
	}

	return packet;
}

template <typename T>
T getPropsPacket(SceneObject& so)
{
	T packet;

	// Loop through all dirty attributes and add them to the packet.
	for (auto& attribute : ObjectAttributes::Dirty(so.Attributes))
	{
		attribute.UpdateDispatch.Post(attribute.GetId());
		so.Attributes.DirtyUpdateDispatch.Post(attribute.GetId());

		// If our dispatch removed the dirty flag, skip this attribute.
		if (!attribute.GetIsDirty())
			continue;

		attribute.SetIsDirty(false);

		auto attr = packet.add_attributes();
		attr->set_id(attribute.GetId());
		attr->set_name(attribute.GetName());

		switch (attribute.GetType())
		{
		case AttributeType::SIGNED:
			attr->set_as_int(attribute.GetSigned());
			break;
		case AttributeType::UNSIGNED:
			attr->set_as_uint(attribute.GetUnsigned());
			break;
		case AttributeType::FLOAT:
			attr->set_as_float(attribute.GetFloat());
			break;
		case AttributeType::DOUBLE:
			attr->set_as_double(attribute.GetDouble());
			break;
		default:
		case AttributeType::STRING:
			attr->set_as_string(attribute.GetString());
			break;
		}
	}

	for (auto& [id, property] : so.Properties)
	{
		if (!property->GetIsDirty())
			continue;

		property->UpdateDispatch.Post(id);

		// If our dispatch removed the dirty flag, skip this property.
		if (!property->GetIsDirty())
			continue;

		property->SetIsDirty(false);

		auto prop = packet.add_properties();
		prop->set_id(id);
		// prop->set_name(property->GetName());

		switch (property->GetType())
		{
		case AttributeType::SIGNED:
			prop->set_as_int(property->GetSigned());
			break;
		case AttributeType::UNSIGNED:
			prop->set_as_uint(property->GetUnsigned());
			break;
		case AttributeType::FLOAT:
			prop->set_as_float(property->GetFloat());
			break;
		case AttributeType::DOUBLE:
			prop->set_as_double(property->GetDouble());
			break;
		default:
		case AttributeType::STRING:
			prop->set_as_string(property->GetString());
			break;
		}
	}

	return packet;
}

} // end namespace tdrp
