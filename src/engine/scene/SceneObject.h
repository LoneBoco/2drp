#pragma once

#include <deque>
#include <any>
#include <unordered_set>

#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>

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


#define _PROPDEF(name, type) \
virtual type Get##name() const; \
virtual void Set##name(const type&);

class SceneObject : public ComponentEntity, public std::enable_shared_from_this<SceneObject>
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
	static constexpr SceneObjectID LocalIDFlag = 0x80'00'00'00;
	static constexpr SceneObjectID FixedIDFlag = 0x40'00'00'00;

public:
	SceneObject(const std::shared_ptr<ObjectClass> c, const SceneObjectID id);
	virtual ~SceneObject();

	SceneObject& operator=(const SceneObject& other);

	SceneObject() = delete;
	SceneObject(const SceneObject& other) = delete;
	SceneObject(SceneObject&& other) = delete;
	SceneObject& operator=(SceneObject&& other) = delete;

public:
	std::weak_ptr<const ObjectClass> GetClass() const noexcept;
	std::shared_ptr<ObjectClass> GetClass();
	PlayerID GetOwningPlayer() const noexcept;
	std::weak_ptr<scene::Scene> GetCurrentScene() const noexcept;
	bool IsOwnedBy(PlayerID id) const noexcept;
	bool IsOwnedBy(std::shared_ptr<server::Player> player) const noexcept;

	void SetOwningPlayer(PlayerID player);
	void SetOwningPlayer(std::shared_ptr<server::Player> player);
	bool SetCurrentScene(std::shared_ptr<scene::Scene> scene);

	void AttachTo(std::shared_ptr<SceneObject> other);
	SceneObjectID GetAttachedId() const noexcept;
	const std::vector<std::weak_ptr<SceneObject>>& AttachedObjects() const noexcept;

public:
	const std::optional<physics::BodyConfiguration>& GetPhysicsBodyConfiguration() const noexcept;
	const std::optional<playrho::BodyID>& GetPhysicsBody() const noexcept;
	bool HasPhysicsBody() const noexcept;
	void SetPhysicsBody(const physics::BodyConfiguration& config) noexcept;
	void SetHybridBodyMoving(bool is_moving) noexcept;
	void SetHybridBodyMoving(playrho::d2::World& world, bool is_moving) noexcept;

public:
	void ConstructPhysicsBodiesFromConfiguration() noexcept;
	virtual void DestroyAllPhysicsBodies() noexcept;
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
	std::shared_ptr<SceneObject> GetThis() noexcept
	{
		return shared_from_this();
	}

	virtual SceneObjectType GetType() const noexcept
	{
		return SceneObjectType::DEFAULT;
	}

	virtual Rectf GetBounds() const noexcept;

	bool IsLocal() const noexcept
	{
		return (ID & LocalIDFlag) != 0;
	}
	bool HasFixedID() const noexcept
	{
		return (ID & FixedIDFlag) != 0;
	}

public:
	const SceneObjectID ID;
	std::string Name;
	bool Initialized = false;
	bool Visible = true;
	bool ReplicateChanges = true;
	bool IgnoresEvents = false;

	ObjectAttributes Attributes;
	ObjectProperties Properties;

	std::string ClientScript;
	std::string ServerScript;

	bool PhysicsChanged = false;

protected:
	virtual void addPhysicsBodyToScene(std::shared_ptr<scene::Scene>& scene) noexcept;

protected:
	const std::shared_ptr<ObjectClass> m_object_class;
	PlayerID m_owning_player = NO_PLAYER;
	std::weak_ptr<scene::Scene> m_current_scene;
	std::weak_ptr<SceneObject> m_attached_to;
	std::vector<std::weak_ptr<SceneObject>> m_attached;
	std::optional<playrho::BodyID> m_physics_body;
	std::optional<physics::BodyConfiguration> m_physics_body_config;
};

using SceneObjectPtr = std::shared_ptr<SceneObject>;

inline std::weak_ptr<const ObjectClass> SceneObject::GetClass() const noexcept
{
	return m_object_class;
}
inline std::shared_ptr<ObjectClass> SceneObject::GetClass()
{
	return m_object_class;
}
inline PlayerID SceneObject::GetOwningPlayer() const noexcept
{
	return m_owning_player;
}
inline std::weak_ptr<scene::Scene> SceneObject::GetCurrentScene() const noexcept
{
	return m_current_scene;
}
inline const std::vector<std::weak_ptr<SceneObject>>& SceneObject::AttachedObjects() const noexcept
{
	return m_attached;
}
inline const std::optional<physics::BodyConfiguration>& SceneObject::GetPhysicsBodyConfiguration() const noexcept
{
	return m_physics_body_config;
}
inline const std::optional<playrho::BodyID>& SceneObject::GetPhysicsBody() const noexcept
{
	return m_physics_body;
}
inline bool SceneObject::HasPhysicsBody() const noexcept
{
	return m_physics_body.has_value();
}

/////////////////////////////

class StaticSceneObject : public SceneObject
{
public:
	StaticSceneObject(const std::shared_ptr<ObjectClass> c, const SceneObjectID id) : SceneObject(c, id)	{}
	virtual ~StaticSceneObject() {}

	SceneObjectType GetType() const noexcept override
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

	virtual SceneObjectType GetType() const noexcept override
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

	virtual SceneObjectType GetType() const noexcept override
	{
		return SceneObjectType::TILEMAP;
	}

public:
	virtual Rectf GetBounds() const noexcept override
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
	struct Chunk
	{
		Vector2di Position;
		Vector2du Size;
		bool TilesLoaded = false;
		bool CollisionLoaded = false;
		bool ChunkRequested = false;
		std::vector<playrho::FixtureID> Fixtures;
	};

public:
	TMXSceneObject(const std::shared_ptr<ObjectClass> c, const SceneObjectID id) : SceneObject(c, id) {}
	virtual ~TMXSceneObject() {}

	TMXSceneObject& operator=(const TMXSceneObject& other);

	virtual SceneObjectType GetType() const noexcept override
	{
		return SceneObjectType::TMX;
	}

public:
	virtual Rectf GetBounds() const noexcept override;
	std::shared_ptr<tmx::Map> GetMap() const noexcept
	{
		return m_tmx;
	}

public:
	bool LoadMap(const std::shared_ptr<tmx::Map>& map, const std::vector<std::pair<scene::TilesetGID, scene::TilesetPtr>>& tilesets) noexcept;
	void LoadChunkData(size_t chunk_idx, size_t chunks_max, Vector2di&& position, Vector2du&& size) noexcept;

public:
	void LoadChunkCollision(uint32_t chunk_idx, std::shared_ptr<scene::Scene> scene = nullptr) noexcept;
	void SetChunkCollision(uint32_t chunk_idx, const physics::BodyConfiguration& config) noexcept;
	void CalculateLevelSize() noexcept;
	void CalculateChunksToRender(const Rectf& window, std::vector<uint32_t>& chunks_to_render) const noexcept;

public:
	virtual void DestroyAllPhysicsBodies() noexcept override;

protected:
	virtual void addPhysicsBodyToScene(std::shared_ptr<scene::Scene>& scene) noexcept override;

public:
	std::vector<std::pair<scene::TilesetGID, scene::TilesetPtr>> Tilesets;
	std::vector<Chunk> Chunks;
	size_t Layer = 0;
	tmx::RenderOrder RenderOrder = tmx::RenderOrder::RightDown;

protected:
	Rectf m_calculated_level_bounds;
	std::shared_ptr<tmx::Map> m_tmx;
	std::vector<uint32_t> m_shape_chunks;
};

/////////////////////////////

class TextSceneObject : public SceneObject
{
public:
	TextSceneObject(const std::shared_ptr<ObjectClass> c, const SceneObjectID id) : SceneObject(c, id), m_font_size(12) {}
	virtual ~TextSceneObject() {}

	SceneObjectType GetType() const noexcept override
	{
		return SceneObjectType::TEXT;
	}

public:
	const std::string& GetFont() const noexcept;
	void SetFont(const std::string& font);

	uint32_t GetFontSize() const noexcept;
	void SetFontSize(uint32_t size);

	bool GetCentered() const noexcept;
	void SetCentered(bool centered);

protected:
	std::string m_font;
	uint32_t m_font_size;
	bool m_centered = false;
};

///////////////////////////////////////////////////////////////////////////////

} // end namespace tdrp
