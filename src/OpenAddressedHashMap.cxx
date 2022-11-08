#include "OpenAddressedHashMap.hxx"

#include <unordered_map>

int main() {
  auto map = std::unordered_map<int, int>();
  auto lhm = nhzaci::open_addressed_hash_map<int, int>();
}
