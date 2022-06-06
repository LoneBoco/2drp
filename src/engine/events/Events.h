#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>

namespace tdrp
{

class EventHandleBase;

// TODO:
// This interface is completely useless except to work around compiler limitations with
// forward declarations and template types.
// There might be a better way, but this works for now.
class EventDispatcherBase
{
public:
	EventDispatcherBase();

	/**
	 * Cleans up any listeners that haven't been unsubscribed yet.
	 */
	virtual ~EventDispatcherBase();

	/**
	 * Unsubscribe an event handler by naked pointer.
	 *
	 * @param handle Naked pointer to the event handle
	 * @return True if successful, false is event handler wasn't registered.
	 */
	bool Unsubscribe(EventHandleBase* handle);

	/**
	 * Unsubscribe an event handler by shared pointer.
	 *
	 * @param handle Shared pointer to the event handle
	 * @return True if successful, false is event handler wasn't registered.
	 */
	bool Unsubscribe(std::shared_ptr<EventHandleBase> handle);

	/**
	 * Unsubscribe all event handlers.
	 */
	void UnsubscribeAll();

protected:
	std::unordered_map<uint32_t, std::weak_ptr<EventHandleBase>> m_eventHandlers;
};

// This class is only to hide the template of the
// event handle implementation.
// Also implemented in cpp file to work around compiler order limitations.
class EventHandleBase
{
	friend class EventDispatcherBase;

public:
	virtual ~EventHandleBase();

protected:
	EventHandleBase(EventDispatcherBase* dispatcher, uint32_t id);

	EventDispatcherBase* m_dispatcher;
	uint32_t m_eventId;
};

template <typename ...A>
class EventHandleImpl : public EventHandleBase
{
public:
	EventHandleImpl(EventDispatcherBase* dispatcher, uint32_t id, std::function<void(A...)> callback)
		: m_callback(callback), EventHandleBase(dispatcher, id)
	{};

	~EventHandleImpl()
	{};

	/**
	 * Dispatches this event handler.
	 *
	 * @param args The arguments to dispatch to this event handler call.
	 */
	void Dispatch(A ...args)
	{
		if (m_callback)
			m_callback(args...);
	};

private:
	std::function<void(A...)> m_callback;
};

// Cheeky typedef so the shared_ptr implementation is transparent to user
typedef std::shared_ptr<EventHandleBase> EventHandle;

template <typename ...A>
class EventDispatcher : public EventDispatcherBase
{
public:
	EventDispatcher() {};
	virtual ~EventDispatcher() {};

	/**
	 * Post an event to the event dispatcher.
	 * This will propagate to all event listeners.
	 *
	 * @param args Arguments to pass to the event handlers.
	 */
	void Post(A ...args)
	{
		for (auto& handler : m_eventHandlers)
		{
			auto ptr = std::static_pointer_cast<EventHandleImpl<A...>>(handler.second.lock());
			if (ptr)
				ptr->Dispatch(args...);
		}
	};

	/**
	 * Subscribe a callback function with the event dispatcher.
	 *
	 * @param callback The function to call back with.
	 * @return An event handle to keep track of your callback function.
	 */
	EventHandle Subscribe(std::function<void(A...)> callback)
	{
		// TODO:
		// Handle rollover later. Maybe create a UUID generator?

		// Static handle counter will give us unique ID's.
		static uint32_t handleCounter = 0;

		// Get handle ID by incrementing the handle counter :)
		auto handleId = handleCounter++;

		// Create an event handle and store it with the handle ID as the key
		std::shared_ptr<EventHandleImpl<A...>> handle = std::make_shared<EventHandleImpl<A...>>(this, handleId, callback);

		// Store a weak pointer to the event handler so we don't increment ref. count
		m_eventHandlers[handleId] = std::weak_ptr<EventHandleBase>(std::static_pointer_cast<EventHandleBase>(handle));

		return handle;
	};
};

} // end namespace tdrp
