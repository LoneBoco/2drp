#include <sstream>

#include "engine/common.h"

#include "SceneObject.h"
#include "ObjectAttributes.h"
#include "ObjectProperties.h"

namespace tdrp
{

ObjectProperties::ObjectProperties()
{
	create();
}

ObjectProperties::ObjectProperties(const ObjectProperties& properties)
{
	create();
	clone(properties);
}

ObjectProperties::~ObjectProperties()
{
}

ObjectProperties& ObjectProperties::operator=(const ObjectProperties& properties)
{
	clone(properties);
	return *this;
}

const Attribute& ObjectProperties::operator[](const Property prop) const
{
	auto a = Get(prop);
	return *a;
}

Attribute& ObjectProperties::operator[](const Property prop)
{
	auto a = Get(prop);
	return *a;
}

std::shared_ptr<const Attribute> ObjectProperties::Get(const Property prop) const
{
	return m_properties.Get((uint16_t)prop);
}

std::shared_ptr<const Attribute> ObjectProperties::Get(const std::string& prop) const
{
	return m_properties.Get(prop);
}

std::shared_ptr<Attribute> ObjectProperties::Get(const Property prop)
{
	return m_properties.Get((uint16_t)prop);
}

std::shared_ptr<Attribute> ObjectProperties::Get(const std::string& prop)
{
	return m_properties.Get(prop);
}

void ObjectProperties::create()
{
// Add 10 to move the pointer past 'Property::' because I am horrible.
#define _PROP(x, y) m_properties.AddAttribute(#x + 10, y, (int32_t)x);

	_PROP(Property::LEVEL, "");
	_PROP(Property::X, 0.0f);
	_PROP(Property::Y, 0.0f);
	_PROP(Property::Z, 0.0f);
	_PROP(Property::SCALE_X, 1.0f);
	_PROP(Property::SCALE_Y, 1.0f);
	_PROP(Property::VELOCITY_X, 0.0f);
	_PROP(Property::VELOCITY_Y, 0.0f);
	_PROP(Property::FORCE_X, 0.0f);
	_PROP(Property::FORCE_Y, 0.0f);
	_PROP(Property::TORQUE, 0.0f);
	_PROP(Property::ROTATION, 0.0f);
	//_PROP(Property::ROTATION_VELOCITY, 0.0f);
	_PROP(Property::DIRECTION, (uint64_t)0);
	_PROP(Property::IMAGE, "");

#undef _PROP
}

void ObjectProperties::clone(const ObjectProperties& properties)
{
	m_properties = properties.m_properties;
}

/*
bool ObjectProperties::parse(rha::net::packet& p)
{
	while (p.has_unread())
	{
		// Get our property attribute.
		Property id = (Property)p.READ1();
		Attribute* attribute = Properties.get(id);
		if (attribute == nullptr)
			return false;

		// Set the attribute.
		attribute->set(p);

		// Call our handler function.
		if (handle != nullptr)
			handle(id, p);
	}
	return true;
}
*/

// static
/*
void ObjectProperties::update(CSceneObject& obj, Property prop)
{
	Attribute* attribute = obj.Properties.get(prop);
	if (attribute == nullptr)
		return;

	switch (prop)
	{
		default:
		case EP_INVALID:
			break;

		case EP_LEVEL:
			break;

		case EP_X:
		case EP_Y:
		case EP_Z:
			// Will be handled separately.
			break;

		case EP_DIRECTION:
			if (obj.get_type() == rha::ESOT_ANIMATED)
			{
				CAnimatedSceneObject* a_obj = (CAnimatedSceneObject*)&obj;
				a_obj->Animation.set_direction((u32)attribute->get_as_uint());
			}
			break;

		case EP_ANIMATION:
		{
			switch (obj.get_type())
			{
				case rha::ESOT_ANIMATED:
				{
					C2DAnimation* anim = CGame::Instance().get_2D_animation(attribute->get_as_string());
					if (anim != nullptr)
					{
						CAnimatedSceneObject* a_obj = (CAnimatedSceneObject*)&obj;
						anim->loadIntoAnimatedSceneNode(&a_obj->Animation, &obj.Attributes);
						a_obj->Animation.IsStopped = false;
					}
					break;
				}
				case rha::ESOT_STATIC:
				{
					C2DImage* image = CGame::Instance().get_2D_image(attribute->get_as_string());
					if (image != nullptr)
					{
						CStaticSceneObject* s_obj = (CStaticSceneObject*)&obj;
						s_obj->StaticImage.setImage(image);
					}
				}
			}
			break;
		}
	}
}
*/

} // end namespace tdrp
