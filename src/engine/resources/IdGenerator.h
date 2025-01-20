#pragma once

#include <concepts>
#include <set>

namespace tdrp
{

template <std::integral T>
class IdGenerator
{
public:
	IdGenerator() = default;
	IdGenerator(T start_id) : m_next_id(start_id) {}

	/// <summary>
	/// Gets the next available id.
	/// </summary>
	/// <returns>The next id.</returns>
	T GetNextId()
	{
		if (!m_free_ids.empty())
		{
			T id = *m_free_ids.begin();
			m_free_ids.erase(m_free_ids.begin());
			return id;
		}
		return m_next_id++;
	}

	/// <summary>
	/// Gets the next id without trying to reuse a released id.
	/// </summary>
	/// <returns>The next id.</returns>
	T GetNextUnusedId()
	{
		return m_next_id++;
	}

	/// <summary>
	/// Peeks at the next available id.
	/// </summary>
	/// <returns>The next id.</returns>
	T PeekNextId() const
	{
		return m_next_id;
	}

	/// <summary>
	/// Peeks at the next available unused id.
	/// </summary>
	/// <returns>The next id.</returns>
	T PeekNextUnusedId() const
	{
		return m_next_id;
	}

	/// <summary>
	/// Sets the next id.
	/// </summary>
	/// <param name="id"></param>
	void SetNextUnusedId(T id)
	{
		m_next_id = id;
	}

	/// <summary>
	/// Releases an id back to the pool.
	/// </summary>
	/// <param name="id">The id to free up.</param>
	void FreeId(T id)
	{
		m_free_ids.insert(id);
	}

	/// <summary>
	/// Resets the state of the id generator.
	/// </summary>
	/// <param name="next_id">The next id to assign.</param>
	void Reset(T next_id = 0)
	{
		m_free_ids.clear();
		m_next_id = next_id;
	}

protected:
	T m_next_id = static_cast<T>(0);
	std::set<T> m_free_ids;
};

} // end namespace tdrp
