#include "Events.h"

namespace tdrp
{

EventHandleBase::~EventHandleBase()
{
	if (m_dispatcher)
		m_dispatcher->Unsubscribe(this);
}

EventHandleBase::EventHandleBase(EventDispatcherBase* dispatcher, uint32_t id)
	: m_dispatcher(dispatcher), m_eventId(id)
{
};

EventDispatcherBase::EventDispatcherBase()
{
}

EventDispatcherBase::~EventDispatcherBase()
{
	// Kill any handles just in case :)
	UnsubscribeAll();
}

bool EventDispatcherBase::Unsubscribe(EventHandleBase* handle)
{
	if (!handle)
		return false;

	// Check if the event handle event exists
	auto itr = m_eventHandlers.find(handle->m_eventId);
	if (itr == m_eventHandlers.end())
		return false;

	// Unregister the event handler.
	// Remove the parent dispatcher just in case
	// it was manually unsubscribed instead of deleted.
	// Otherwise you might get a double delete.
	handle->m_dispatcher = nullptr;
	m_eventHandlers.erase(itr);

	return true;
};

bool EventDispatcherBase::Unsubscribe(std::shared_ptr<EventHandleBase> handle)
{
	if (handle)
		return Unsubscribe(handle.get());
	else
		return false;
};

void EventDispatcherBase::UnsubscribeAll()
{
	for (auto& handler : m_eventHandlers)
	{
		auto ptr = handler.second.lock();
		if (ptr)
			ptr->m_dispatcher = nullptr;
	}

	m_eventHandlers.clear();
}

} // end namespace tdrp
