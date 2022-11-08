#pragma once

#include <cstddef>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <utility>

#include "MapNode.hxx"
#include "Probe.hxx"

namespace nhzaci {

/**
 * @brief Simple Hash Map that utilizes the linear probing
 * algorithm to try and find items
 *
 * @tparam Key        Type of key objects
 * @tparam T          Type of mapped objects
 * @tparam Hash       Hashing function object type, defaults to std::hash<T>
 * @tparam KeyEqual   Predicate function object type, defaults to
 * std::equal_to<Key>
 * @tparam Allocator  defaults to std::allocator<MapNode<const Key, T>>
 * @tparam Probe      Probe algorithm function object type, defaults to probe<T>
 * which linearly probes
 */
template <typename Key, typename T, typename Hash = std::hash<T>,
          typename KeyEqual = std::equal_to<Key>,
          typename Allocator = std::allocator<MapNode<const Key, T>>,
          typename Probe = probe<Key, MapNode<Key, T>, Hash, KeyEqual>>
class open_addressed_hash_map {
public:
  class Iterator {
  public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = MapNode<Key, T>;
    using pointer = value_type *;
    using reference = value_type &;

    Iterator(pointer ptr) : m_ptr{ptr} {};

    reference operator*() const { return *m_ptr; }
    pointer operator->() { return m_ptr; }

    // Prefix increment
    Iterator &operator++() {
      m_ptr++;
      return *this;
    }
    // Postfix increment
    Iterator &operator++(int) {
      Iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    // Equality operators
    friend bool operator==(const Iterator &a, const Iterator &b) {
      return a.m_ptr == b.m_ptr;
    };
    friend bool operator!=(const Iterator &a, const Iterator &b) {
      return a.m_ptr != b.m_ptr;
    };

  private:
    pointer m_ptr;
  };

  // member types as required on cppreference
  using key_type = Key;
  using mapped_type = T;
  using value_type = MapNode<Key, T>;
  using size_type = size_t;
  using difference_type = std::ptrdiff_t;
  using hasher = Hash;
  using key_equal = KeyEqual;
  using allocator_type = Allocator;
  using reference = value_type &;
  using const_reference = const value_type &;
  using alloc_traits_t = std::allocator_traits<Allocator>;
  using pointer = typename alloc_traits_t::pointer;
  using const_pointer = typename alloc_traits_t::const_pointer;
  using iterator = Iterator;
  using const_iterator = const Iterator;
  using prober = Probe;

  iterator begin() { return Iterator(&(container_p_[0])); }
  iterator end() { return Iterator(&(container_p_[max_size_])); }

  const_iterator begin() const { return Iterator(&(container_p_[0])); }
  const_iterator end() const { return Iterator(&(container_p_[max_size_])); }

  /////////////////////
  // Special Mem Fns //
  /////////////////////

  open_addressed_hash_map()
      : container_p_{nullptr}, max_size_{0}, curr_size_{0},
        max_load_factor_{0.75}, hash_fn_{hasher()}, probe_fn_{prober()},
        key_eq_fn_{key_equal()}, alloc_{} {};
  ~open_addressed_hash_map() {
    if (container_p_ != nullptr) {
      // TODO: Deallocate from alloc instead of delete
      // alloc_traits_t::deallocate(alloc_, container_p_, max_size_);
      delete[] container_p_;
    }
  }

  // deleted as copying could lead to double-free
  open_addressed_hash_map(const open_addressed_hash_map &) = delete;
  open_addressed_hash_map &operator=(const open_addressed_hash_map &) = delete;

  // moving is fine though!
  open_addressed_hash_map(open_addressed_hash_map &&other) {
    container_p_ = other.container_p_;
    max_size_ = other.max_size_;
    curr_size_ = other.curr_size_;
    max_load_factor_ = 0.75;
    hash_fn_ = other.hash_fn_;
    alloc_ = other.alloc_;

    other.container_p_ = nullptr;
    other.curr_size_ = 0;
    other.max_size_ = 0;
  }
  open_addressed_hash_map &operator=(open_addressed_hash_map &&other) {
    *this(std::move(other));
  }

  /////////////////////
  // Capacity  start //
  /////////////////////

  bool empty() { return curr_size_ == 0; }

  size_t size() { return curr_size_; }

  size_t max_size() { return max_size_; }

  /////////////////////
  // Capacity  end   //
  /////////////////////

  /////////////////////
  // Modifiers start //
  /////////////////////

  // TODO: Deallocate with alloc
  void clear() {
    if (container_p_ == nullptr)
      return;

    // alloc_traits_t::deallocate(alloc_, container_p_, max_size_);
    delete[] container_p_;
    container_p_ = nullptr;
    curr_size_ = 0;
    max_size_ = 0;
  }

  // TODO: Deallocate with alloc
  std::pair<iterator, bool> insert(const value_type &value) {
    expand_container_if_load_factor_reached();
    auto idx = get_empty_bucket_index(value.key);

    // TODO: I don't really like this additional embedded null check in
    // insert, it's unexpected behaviour from the standpoint of a user as it
    // adds overhead, on the flip side, having duplicate keys lead to UB as
    // well because if we find multiple keys, we only ever return the first
    // key which becomes a problem since the other key values will live on
    // forever in the hash map but never get retrieved...
    //
    // avoid memory leak, if the existing pointer is not empty, we should free
    // it first before replacing it and leaving a dangling pointer
    if (container_p_[idx].t_p != nullptr) {
      delete container_p_[idx].t_p;
      curr_size_--;
    }

    container_p_[idx] = value;
    curr_size_++;
    return {Iterator(&(container_p_[idx])), true};
  }

  // TODO: Deallocate with alloc
  std::pair<iterator, bool> insert(value_type &&value) {
    expand_container_if_load_factor_reached();
    auto idx = get_empty_bucket_index(value.key);

    // avoid memory leak, if the existing pointer is not empty, we should free
    // it first before replacing it and leaving a dangling pointer
    if (container_p_[idx].t_p != nullptr) {
      delete container_p_[idx].t_p;
      curr_size_--;
    }

    container_p_[idx] = move(value);
    curr_size_++;
    return {Iterator(&(container_p_[idx])), true};
  }

  // template <class M>
  // std::pair<iterator, bool> insert_or_assign( const Key& k, M&& obj );

  // commented out these functions as its not necessary
  // template <class P> std::pair<iterator, bool> insert(P &&p);
  // iterator insert(const_iterator, const value_type &);
  // iterator insert(const_iterator, value_type &&);
  // template <class P> iterator insert(const_iterator, P &&);
  // template <class InputIt> void insert(InputIt first, InputIt last);
  // void insert(std::initializer_list<value_type> ilist);

  template <typename... Args> std::pair<iterator, bool> emplace(Args &&...args);
  // template <typename... Args>
  // iterator emplace_hint(const_iterator hint, Args &&...args);
  // template< class... Args >
  // pair<iterator, bool> try_emplace( const Key& k, Args&&... args );

  iterator erase(const_iterator pos);
  iterator erase(const_iterator first, const_iterator last);
  size_type erase(const key_type &key);
  template <typename K> size_type erase(K &&x);

  void swap(open_addressed_hash_map &other) noexcept;

  /////////////////////
  // Modifiers end //
  /////////////////////

  /////////////////////
  // Lookup start    //
  /////////////////////

  T &at(const key_type &key) {
    size_type index = find_item_index(key);

    if (index == max_size_) {
      throw std::out_of_range("Key not found in hash map");
    }

    return *container_p_[index].t_p;
  }
  const T &at(const key_type &key) const { return at(key); }

  T &operator[](const key_type &key) { return *(find(key)->t_p); }
  T &operator[](key_type &&key) { return *(find(key)->t_p); }

  size_type count(const key_type &key) const {
    size_type startIdx = find_item_index(key);

    // if out of bound, item is not found
    if (startIdx == max_size_)
      return 0;

    return 1;
  }
  template <typename K> size_type count(const K &x) const { return count(x); }

  iterator find(const key_type &key) {
    auto idx = find_item_index(key);
    return Iterator(&(container_p_[idx]));
  }

  const_iterator find(const key_type &key) const {
    return Iterator(&(container_p_[find_item_index(key)]));
  }

  template <typename K> iterator find(const K &x) { return find(x); }
  template <typename K> const_iterator find(const K &x) const {
    return find(x);
  }

  bool contains(const key_type &key) const { return find(key) != end(); }
  template <typename K> bool contains(const K &x) const { return contains(x); }

  /////////////////////
  // Lookup end      //
  /////////////////////

  ////////////////////////////
  // Hash Policy      start //
  ////////////////////////////

  float load_factor() const {
    return static_cast<float>(curr_size_) / max_size_;
  }

  float max_load_factor() const { return max_load_factor_; }
  void max_load_factor(float ml) { max_load_factor_ = ml; }

  void rehash(size_type count);

  void reserve(size_type count);

  ////////////////////////////
  // Hash Policy      end   //
  ////////////////////////////

  ////////////////////////////
  // Observers        start //
  ////////////////////////////

  hasher hash_function() const { return hash_fn_; }
  key_equal key_eq() const { return key_eq_fn_; }

  ////////////////////////////
  // Observers        end   //
  ////////////////////////////

private:
  value_type *container_p_;
  size_type max_size_;
  size_type curr_size_;
  float max_load_factor_;
  key_equal key_eq_fn_;
  hasher hash_fn_;
  prober probe_fn_;
  allocator_type alloc_;

  // TODO: Use the allocator instead of allocating with new
  void expand_container_if_load_factor_reached() {
    // TODO: Benchmark and test what's a good starting number to have
    // for number of buckets
    if (max_size_ == 0) {
      max_size_ = 4;
      // TODO: Use the allocator instead of allocating with new
      container_p_ = new value_type[max_size_];
      // container_p_ = alloc_traits_t::allocate(alloc_, max_size_);
      // alloc_traits_t::construct(alloc_, container_p_, max_size_);
      return;
    }

    if (static_cast<double>(curr_size_ + 1) / max_size_ <= max_load_factor_)
      return;

    auto newSize = max_size_ * 2;
    // auto newContainer = alloc_traits_t::allocate(alloc_, newSize);
    auto newContainer = new value_type[newSize];
    rehash_into_new_container(container_p_, max_size_, newContainer, newSize);
    delete[] container_p_;
    // alloc_traits_t::deallocate(container_p_, max_size_);

    container_p_ = newContainer;
    max_size_ = newSize;
  }

  void rehash_into_new_container(value_type *old_container, size_t old_size,
                                 value_type *new_container, size_t new_size) {
    for (size_t i = 0; i < old_size; i++) {
      if (container_p_[i].t_p == nullptr)
        continue;

      auto newBucketIndex = probe_fn_.get_empty_bucket_index(
          new_container, new_size, container_p_[i].key, hash_fn_, key_eq_fn_);
      new_container[newBucketIndex] = container_p_[i];
      container_p_[i].t_p = nullptr;
    }
  }

  size_t get_empty_bucket_index(const key_type &key) const {
    return probe_fn_.get_empty_bucket_index(container_p_, max_size_, key,
                                            hash_fn_, key_eq_fn_);
  }

  size_t find_item_index(const key_type &key) const {
    return probe_fn_.find_item_key(container_p_, max_size_, key, hash_fn_,
                                   key_eq_fn_);
  }
};

}; // namespace nhzaci
