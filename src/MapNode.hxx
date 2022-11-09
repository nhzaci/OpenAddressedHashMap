#pragma once

#include <type_traits>

namespace nhzaci {

/**
 * @brief MapNode holds the key and pointer to a value in a hash map
 *
 * @tparam Key  Type of key objects, must be default contructible
 * @tparam T    Type of mapped objects
 */
template <typename Key, typename T, typename Deleter = std::default_delete<T>>
struct MapNode {
  static_assert(std::is_default_constructible_v<Key>,
                "Key of a MapNode must be default constructible");
  static_assert(std::is_default_constructible_v<T>,
                "Mapped object T of a MapNode must be default constructible");

  // TODO: Add template concept for Key to be default constructible
  MapNode() : key{}, t_p{nullptr} {};

  // used in operator[] of hash map, default construct with some key
  // and default construction of T
  MapNode(Key k) : key{k}, t_p{new T()} {};

  MapNode(Key k, T *t) : key{k}, t_p{t} {};

  MapNode(const MapNode &other) {
    key = other.key;
    t_p = new T(*other.t_p);
  }
  MapNode &operator=(const MapNode &other) {
    key = other.key;
    t_p = new T(*other.t_p);
    return *this;
  }

  MapNode(MapNode &&other) : key{other.key}, t_p{other.t_p} {
    other.t_p = nullptr;
  }
  MapNode &operator=(MapNode &&other) {
    key = other.key;
    t_p = other.t_p;

    other.t_p = nullptr;
    return *this;
  }

  Key key;
  T *t_p;

  ~MapNode() {
    if (t_p != nullptr)
      Deleter()(t_p);
  }

  friend inline std::ostream &operator<<(std::ostream &os,
                                         const MapNode &node) {
    return os << "MapNode(key=" << node.key << ";t_p=" << node.t_p << ")";
  }
};

}; // namespace nhzaci