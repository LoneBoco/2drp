#pragma once

#include <deque>
#include <set>

#include <Box2D/Box2D.h>

#include "engine/common.h"

// #include "Managers/CPhysicsManager.h"
#include "engine/scene/ObjectClass.h"
#include "engine/scene/ObjectAttributes.h"
#include "engine/scene/ObjectProperties.h"
#include "engine/scene/Tileset.h"

#include "engine/script/Script.h"

#include "engine/component/Component.h"

namespace tdrp
{

//! The type of a scene object.
enum class SceneObjectType
{
	DEFAULT = 0,
	STATIC,
	ANIMATED,
	TILED,

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

constexpr uint32_t GlobalSceneObjectIDFlag = 0x80'00'00'00;

class SceneObject : public ComponentEntity
{
	SCRIPT_FUNCTION(OnCreated)
	SCRIPT_FUNCTION(OnUpdate)
	SCRIPT_FUNCTION(OnEvent)

public:
	//! Constructor.
	SceneObject(const std::shared_ptr<ObjectClass> c, const uint32_t id);

	//! Destructor.
	virtual ~SceneObject();

	SceneObject() = delete;
	SceneObject(const SceneObject& other) = delete;
	SceneObject(SceneObject&& other) = delete;
	SceneObject& operator=(const SceneObject& other) = delete;
	SceneObject& operator=(SceneObject&& other) = delete;

	//! Gets the class of the object.
	//! \return A weak pointer to a const class.
	std::weak_ptr<const ObjectClass> GetClass() const
	{
		return m_object_class;
	}

	//! Gets the class of the object.
	//! \return A pointer to the class.
	std::shared_ptr<ObjectClass> GetClass()
	{
		return m_object_class;
	}

	//! Returns the scene object type.
	virtual SceneObjectType GetType() const
	{
		return SceneObjectType::DEFAULT;
	}

	//! Returns the position of the scene object.
	virtual Vector2df GetPosition() const;

	//! Sets the position of the scene object.
	//! \param position The new position to set.
	virtual void SetPosition(const Vector2df& position);

	//! Gets the depth of the scene object (Z).
	//! \return The depth.
	virtual int64_t GetDepth() const;

	//! Sets the depth of the scene object (Z).
	//! \param z The depth.
	virtual void SetDepth(int64_t z);

	//! Returns the rotation of the scene object.
	virtual float GetRotation() const;

	//! Sets the rotation of the scene object.
	//! \param rotation The rotation to set.
	virtual void SetRotation(float rotation);

	//! Gets the scale of the scene object.
	//! \return The scale.
	virtual Vector2df GetScale() const;

	//! Sets the scale of the scene object.
	//! \param scale The new scale.
	virtual void SetScale(const Vector2df& scale);

	//! Returns the image.
	virtual std::string GetImage()
	{
		return std::string();
	}

	//! Sets the image of the scene node.
	virtual void SetImage(const std::string& image)
	{
		// TODO: Log an error if this happens.
	}

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
	void Update();

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
	//! Returns if the object is global or not.
	bool IsGlobal()
	{
		return (ID & GlobalSceneObjectIDFlag) != 0;
	}

	//! ID
	const uint32_t ID;

	//! Name.
	std::string Name;

	//! Attributes.
	ObjectAttributes Attributes;

	//! Properties.
	ObjectProperties Properties;

	//! Physics.
	// physics::Physics Physics;
	
	//! Sets visibility.
	bool Visible;

	//! Set to true if it was visible during the render state.
	bool RenderVisible;

protected:
	const std::shared_ptr<ObjectClass> m_object_class;
	// FSceneObjectUpdate UpdateCallback;
	// FSceneObjectUpdate PhysicsUpdateCallback;
	// b2Vec2 PreviousPhysicsPosition;
};

class StaticSceneObject : public SceneObject
{
public:
	//! Constructor.
	StaticSceneObject(const std::shared_ptr<ObjectClass> c, const uint32_t id)
	: SceneObject(c, id)
	{
	}

	//! Destructor.
	virtual ~StaticSceneObject()
	{
	}

	//! Returns the scene object type.
	SceneObjectType GetType() const override
	{
		return SceneObjectType::STATIC;
	}

	//! Returns the image.
	std::string GetImage() override
	{
		return Properties.Get(Property::IMAGE)->GetString();
	}

	//! Sets the image of the scene node.
	void SetImage(const std::string& image) override
	{
		std::string old_image = Properties.Get(Property::IMAGE)->GetString();

		if (old_image != image)
			Properties[Property::IMAGE] = image;
	}
};

class AnimatedSceneObject : public SceneObject
{
public:
	//! Constructor.
	AnimatedSceneObject(const std::shared_ptr<ObjectClass> c, const uint32_t id)
	: SceneObject(c, id)
	{
	}

	//! Destructor.
	virtual ~AnimatedSceneObject()
	{
	}

	//! Returns the scene object type.
	virtual SceneObjectType GetType() const override
	{
		return SceneObjectType::ANIMATED;
	}

	//! Returns the image.
	virtual std::string GetImage() override
	{
		return Properties.Get(Property::IMAGE)->GetString();
	}

	//! Sets the image of the scene node.
	virtual void SetImage(const std::string& image) override
	{
		std::string old_image = Properties.Get(Property::IMAGE)->GetString();

		if (old_image != image)
			Properties[Property::IMAGE] = image;
	}
};

class TiledSceneObject : public SceneObject
{
public:
	//! Constructor.
	TiledSceneObject(const std::shared_ptr<ObjectClass> c, const uint32_t id)
	: SceneObject(c, id)
	{
	}

	//! Destructor.
	virtual ~TiledSceneObject()
	{
	}

	//! Returns the scene object type.
	virtual SceneObjectType GetType() const override
	{
		return SceneObjectType::TILED;
	}

	Vector2di Dimension;
	std::vector<char> TileData;
	std::shared_ptr<scene::Tileset> Tileset;
};

} // end namespace tdrp
