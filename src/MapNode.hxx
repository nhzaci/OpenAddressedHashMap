#pragma once

namespace nhzaci {

/**
 * @brief MapNode holds the key and pointer to a value in a hash map
 *
 * @tparam Key  Type of key objects, must be default contructible
 * @tparam T    Type of mapped objects
 */
template <typename Key, typename T> struct MapNode {
  // TODO: Add template concept for Key to be default constructible
  MapNode() : key{}, t_p{nullptr} {};
  MapNode(Key k, T *t) : key{k}, t_p{t} {};
  Key key;
  T *t_p;

  ~MapNode() {
    // TODO: Use alloc to deallocate
    if (t_p != nullptr)
      delete t_p;
  }

  friend inline std::ostream &operator<<(std::ostream &os,
                                         const MapNode &node) {
    return os << "MapNode(key=" << node.key << ";t_p=" << node.t_p << ")";
  }
};

}; // namespace nhzaci