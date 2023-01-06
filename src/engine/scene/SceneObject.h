#pragma once

#include <deque>
#include <set>
#include <any>

#include <tmxlite/Map.hpp>

#include "engine/common.h"

#include "engine/component/Component.h"
#include "engine/physics/Physics.h"
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

#define _PROPDEF(name, type) \
virtual type Get##name() const; \
virtual void Set##name(const type&);


constexpr SceneObjectID GlobalSceneObjectIDFlag = 0x80'00'00'00;

class SceneObject : public ComponentEntity
{
	SCRIPT_SETUP;
	SCRIPT_FUNCTION(OnCreated);
	SCRIPT_FUNCTION(OnUpdate);
	SCRIPT_FUNCTION(OnEvent);
	SCRIPT_FUNCTION(OnOwnershipChange);
	SCRIPT_FUNCTION(OnAnimationEnd);
	SCRIPT_FUNCTION(OnAttributeChange);
	SCRIPT_FUNCTION(OnAttached);
	SCRIPT_FUNCTION(OnCreatePhysics);
	SCRIPT_FUNCTION(OnCollision);
	SCRIPT_FUNCTION(OnCollisionEnd);
	SCRIPT_FUNCTION(OnSolveContact);

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
	bool SetCurrentScene(std::shared_ptr<scene::Scene> scene);

	void AttachTo(std::shared_ptr<SceneObject> other);
	SceneObjectID GetAttachedId();

	std::optional<playrho::BodyID> GetInitializedPhysicsBody();
	void SynchronizePhysics();

public:
	_PROPDEF(Position, Vector2df);
	_PROPDEF(Depth, int64_t);
	_PROPDEF(Angle, float);
	_PROPDEF(Scale, Vector2df);
	_PROPDEF(Velocity, Vector2df);
	_PROPDEF(Acceleration, Vector2df);
	_PROPDEF(VelocityAngle, float);
	_PROPDEF(AccelerationAngle, float);
	_PROPDEF(Direction, int64_t);
	_PROPDEF(Image, std::string);
	_PROPDEF(Entity, std::string);
	_PROPDEF(Animation, std::string);
	_PROPDEF(Text, std::string);

public:
	virtual SceneObjectType GetType() const
	{
		return SceneObjectType::DEFAULT;
	}

	virtual Rectf GetBounds() const;

	bool IsGlobal()
	{
		return (ID & GlobalSceneObjectIDFlag) != 0;
	}

public:
	const SceneObjectID ID;
	std::string Name;
	bool Visible = true;
	bool Replicated = true;

	ObjectAttributes Attributes;
	ObjectProperties Properties;

	std::string ClientScript;
	std::string ServerScript;

	std::optional<playrho::BodyID> PhysicsBody;
	bool PhysicsChanged = false;

protected:
	const std::shared_ptr<ObjectClass> m_object_class;
	std::weak_ptr<server::Player> m_owning_player;
	std::weak_ptr<scene::Scene> m_current_scene;
	std::weak_ptr<SceneObject> m_attached_to;
	std::vector<std::weak_ptr<SceneObject>> m_attached;
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

} // end namespace tdrp
