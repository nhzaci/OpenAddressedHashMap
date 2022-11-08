#pragma once

#include <iostream>

namespace nhzaci {

/**
 * @brief Probing function object encapsulates the algorithm required to
 *        carry out open addressing of a hash map.
 *
 * @tparam Key        Type of key object
 * @tparam ValueType  Type of mapped object
 * @tparam Hash       Hash function object from hash map
 * @tparam KeyEqual   Predicate function object from hash map
 */
template <typename Key, typename ValueType, typename Hash, typename KeyEqual>
struct probe {
  using value_type_p = ValueType *;
  using const_value_type_p = const ValueType *;
  using const_hasher_ref = const Hash &;
  using const_key_ref = const Key &;
  using const_key_equal_ref = const KeyEqual &;

  // TODO: Add template concept for ValueType to contain key and t_p;
  // TODO: Add template concept for Hash;
  /**
   * @brief Probe and return index for next empty bucket
   *
   * Implicit assumption: container passed in will never be entirely full,
   * passing in a container that's completely full **will lead to an infinite
   * loop in probing**
   *
   * @param container   pointer to base container of the hash map
   * @param max_size    max number of elements container contains
   * @param key         key of object we are trying to add in
   * @param hash_f      hash function that we are taking in
   * @return size_t     returns first empty bucket from probing algorithm
   */
  size_t get_empty_bucket_index(value_type_p container, size_t max_size,
                                const_key_ref key, const_hasher_ref hash_f,
                                const_key_equal_ref equal_to_f) const noexcept {
    auto bucket = hash_f(key) % max_size;

    // Implicit assumption: buckets will NEVER be completely full and point back
    // to start i.e. starting bucket at 4, but entire array `container` is full,
    // so we wrap around back to 4, this will cause an infinite loop! But if the
    // load factor holds for a hash map, it should ALWAYS expand before it's 1.0
    while (container[bucket].t_p != nullptr and
           not equal_to_f(key, container[bucket].key)) {
      bucket = (bucket + 1) % max_size; // wrap around
    }

    return bucket;
  }

  /**
   * @brief Probe for a certain key, returning max_size if no match is found,
   * this max_size should be then put into an iterator and the invariant that
   *        iterator(max_size) == container.end() MUST be true.
   *
   *        Implicit assumption: container passed in will never be entirely
   * full, passing in a container that's completely full **will lead to an
   * infinite loop in probing**
   *
   * @return size_t     returns first empty bucket from probing algorithm
   * @param container   pointer to base container of the hash map
   * @param max_size    max number of elements container contains
   * @param key         key of object we are trying to add in
   * @param hash_f      hash function that we are taking in
   * @param equal_to_f  equal to function object to check key equality
   * @return size_t     max_size if not found, otherwise index to item
   */
  size_t find_item_key(value_type_p container, size_t max_size,
                       const_key_ref key, const_hasher_ref hash_f,
                       const_key_equal_ref equal_to_f) const noexcept {
    auto hash = hash_f(key);
    auto bucket = hash % max_size;

    // Implicit assumption: there will not be an infinite loop as it eventually
    // reaches a bucket which is null, which is an invariant that should hold
    // true if the load factor is not 1.0
    while (container[bucket].t_p != nullptr) {
      // return key if found
      if (equal_to_f(key, container[bucket].key))
        return bucket;
      bucket = (bucket + 1) % max_size;
    }

    // otherwise we reached a nullptr without finding a match, so no match is
    // found, return max_size which should return an itr end()
    return max_size;
  }
};

}; // namespace nhzaci