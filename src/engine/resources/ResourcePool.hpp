#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <utility>
#include <string>

#include "Resource.hpp"

namespace tdrp {
  class ResourcePool {
  public:
    // TODO: Return ResourceHandle with shared ownership of the resource here
    template <typename T, typename ...A>
    void CreateResource(const std::string& name, A&& ...args) {
      auto underlying = std::make_unique<T>(args...);
      auto resource = std::make_shared< Resource<T> >(name, std::move(underlying));

      m_resources.push_back(std::static_pointer_cast<BaseResource>(resource));
    };

  private:
    std::vector< std::weak_ptr<BaseResource> > m_resources;
  };
}
