#pragma once

#include <memory>
#include <unordered_map>
#include <functional>
#include <vector>
#include <type_traits>

namespace tdrp {
  // ComponentID's uniquely identify a Component
  using ComponentID = int;

  struct ComponentMeta {
    ComponentMeta(const char* name, const ComponentID componentID) :
      name(name), componentID(componentID) {};

    const char* name = nullptr;
    const ComponentID componentID = -1;
  };

  /**
   * Utility structs
   */
  class ComponentEntity;
	template <typename T, typename ...Args>
	struct Util_ComponentItr;

  /**
   * Base class for each Component.
   *
   * Anything that wants to implement Component should derive from this class
   * and also expand the COMPONENT_ENABLE macro inside the class before anything else
   */
  class ComponentEntity;
  class Component {
    friend class ComponentEntity;

  public:
    Component() {}
    virtual ~Component() {}

    // Lifecycle methods
    virtual void Initialize(ComponentEntity& owner) {}

    virtual void OnAttached(ComponentEntity& owner) {}
    virtual void OnDetached(ComponentEntity& owner) {}

    // Metadata accessors
    virtual const ComponentMeta& GetComponentMeta() const = 0;
    virtual ComponentID GetComponentID() const {
      return GetComponentMeta().componentID;
    }

    ComponentEntity& GetOwner() {
      return *m_owner;
    }

  protected:
    ComponentEntity* m_owner = nullptr;

    static ComponentID AllocComponentID() {
      static ComponentID componentIDCounter;

      return componentIDCounter++;
    }

    static ComponentMeta AllocComponentMeta(const char* name) {
      return {
        name,
        AllocComponentID()
      };
    }
  };

  /**
   * ComponentEntity can hold any number of Components
   */
  class ComponentEntity {
  public:
    ComponentEntity() {}
    virtual ~ComponentEntity() {}

		/**
		 * Requires a component as a corequisite ot the current component.
     *
     * @return The newly created component, or the component that already exists.
     */
		template <typename T>
		std::weak_ptr<T> RequireComponent() {
			if (HasComponent<T>()) {
        return GetComponent<T>();
			}

			return AddComponent<T>();
		}

    /**
     * Adds a component by template parameter.
     *
     * @return The newly added component, or if it already exists, the existant one.
     */
    template <typename T>
    std::weak_ptr<T> AddComponent() {
      if (HasComponent<T>()) {
        return GetComponent<T>();
      }

      // Create and add the component
			return std::static_pointer_cast<T>(AddComponent(std::make_shared<T>()).lock());
    }

    /**
     * Adds a component.
     *
     * @return The component that was added, or an empty shared_ptr on failure.
     */
    std::weak_ptr<Component> AddComponent(const std::shared_ptr<Component>& component) {
      // Don't allow adding two of the same component
      if (HasComponent(component->GetComponentMeta().componentID)) {
        return GetComponent(component->GetComponentMeta().componentID);
      }

      // Don't allow the component to be added if it already has an owner
      if (component->m_owner) {
        return std::shared_ptr<Component>();
      }

      component->m_owner = this;
      m_components[component->GetComponentMeta().componentID] = component;

			component->Initialize(*this);

      return component;
    }

    /**
     * Checks if this entity has as specific component type by template parameter.
     *
     * @return true if yes, false if no
     */
    template <typename T>
    bool HasComponent() const {
      return HasComponent(T::GetMeta().componentID);
    }

		/**
     * Checks if this entity has specific components
     *
     * @return true on all match, false if missing
     */
		template <typename ...Args>
 	  bool HasComponents() const {
			return Util_ComponentItr<Args...>::HasComponents(*this);
		}


    /**
     * Checks if this entity has a specific component type
     *
     * @return true if yes, false if no
     */
    bool HasComponent(const ComponentID componentID) const {
      return m_components.find(componentID) != m_components.end();
    }

    /**
     * Gets a component by template parameter, if this ComponentEntity has it.
     *
     * @return a weak_ptr to the component, or an empty weak_ptr
     */
    template <typename T>
    std::weak_ptr<T> GetComponent() const {
      if (!HasComponent<T>()) {
        return std::weak_ptr<T>();
      }

			auto itr = m_components.find(T::GetMeta().componentID);
      return std::weak_ptr(std::static_pointer_cast<T>(itr->second));
    }
    
    /**
     * Gets a component by id, if this ComponentEntity has it.
     *
     * @return a weak_ptr to the component, or an empty weak_ptr
     */
    std::weak_ptr<Component> GetComponent(const ComponentID componentID) const {
      if (!HasComponent(componentID)) {
        return std::weak_ptr<Component>();
      }

			auto itr = m_components.find(componentID);
      return std::weak_ptr(itr->second);
    }

		/**
		 * Execute some function if the entity has these components.
     */
		template <typename ...Components>
		void IfMatchComponentsImpl(const std::function<void(Components&...)>& func) const {
			if (HasComponents<Components...>()) {
				auto args = Util_ComponentItr<Components...>::GetComponentRefs(*this);

				std::apply(func, args);
			}
		}

		template <typename F>
		void IfMatchComponents(const F&& func) const {
			IfMatchComponentsImpl(std::function(func));
		}

  private:
    std::unordered_map<ComponentID, std::shared_ptr<Component>> m_components;
  };

  /**
   * Structs for template sex
   */
  template <typename T, typename ...Args>
  struct Util_ComponentItr {
    static void GetComponentMeta(std::vector<ComponentMeta>& list) {
      list.push_back(T::_2d_com_meta_);

      Util_ComponentItr<Args...>::GetComponentMeta(list);
    }

    static bool HasComponents(const ComponentEntity& ent) {
			return ent.HasComponent<T>() && Util_ComponentItr<Args...>::HasComponents(ent);
		}

		static std::tuple<T&, Args&...> GetComponentRefs(const ComponentEntity& ent) {
			T& component = *ent.GetComponent<T>().lock().get();

			return std::tuple_cat(
        std::forward_as_tuple(component),
        Util_ComponentItr<Args...>::GetComponentRefs(ent)
			);
		}
  };

  template <typename T>
  struct Util_ComponentItr<T> {
    static void GetComponentMeta(std::vector<ComponentMeta>& list) {
      list.push_back(T::_2d_com_meta_);
    }

    static bool HasComponents(const ComponentEntity& ent) {
			return ent.HasComponent<T>();
		}

		static std::tuple<T&> GetComponentRefs(const ComponentEntity& ent) {
			auto& component = *ent.GetComponent<T>().lock().get();

      return std::forward_as_tuple(component);
		}
  };

  template <>
  struct Util_ComponentItr<void, void> {
    static void GetComponentMeta(std::vector<ComponentMeta>& list) {
    }

    static bool HasComponents(const ComponentEntity& ent) {
			return true;
		}

		static void GetComponentRefs(const ComponentEntity& ent) {
		}
  };
}

#define COMPONENT_ENABLE(name) \
static inline ComponentMeta _2d_com_meta_ = tdrp::Component::AllocComponentMeta(#name); \
public: \
static const ComponentMeta& GetMeta() { \
  return _2d_com_meta_; \
} \
\
const ComponentMeta& GetComponentMeta() const override { \
  return name::GetMeta(); \
}
