#include <gtest/gtest.h>
#include <vector>

#include "../src/OpenAddressedHashMap.hxx"

using namespace nhzaci;

class OAHashMapTest : public ::testing::Test {
protected:
  void SetUp() override {
    node1 = new MapNode(1, new int(1));
    node10 = new MapNode(10, new int(100));
    node2 = new MapNode(2, new int(10));
    lhm.insert(*node1);
    lhm.insert(*node10);
    lhm.insert(*node2);
  }

  // void TearDown() override { }

  MapNode<int, int> *node1;
  MapNode<int, int> *node10;
  MapNode<int, int> *node2;
  open_addressed_hash_map<int, int> lhm;
};

TEST_F(OAHashMapTest, lhmCorrectSize) {
  EXPECT_EQ(lhm.empty(), false);
  EXPECT_EQ(lhm.size(), 3);
}

TEST_F(OAHashMapTest, lhmFindNodesWorks) {
  auto node1Itr = lhm.find(node1->key);
  EXPECT_EQ(node1Itr->key, node1->key);
  auto node10Itr = lhm.find(node10->key);
  EXPECT_EQ(node10Itr->key, node10->key);
  auto node2tr = lhm.find(node2->key);
  EXPECT_EQ(node2tr->key, node2->key);
  auto randomNodeNotInMap = MapNode(200, new int(200));
  auto randomNodeItr = lhm.find(randomNodeNotInMap.key);
  EXPECT_TRUE(randomNodeItr == lhm.end());
}

TEST_F(OAHashMapTest, lhmFindNodeNotInMapEqualEndItr) {
  auto randomNodeNotInMap = MapNode(200, new int(200));
  auto randomNodeItr = lhm.find(randomNodeNotInMap.key);
  EXPECT_TRUE(randomNodeItr == lhm.end());
}

TEST_F(OAHashMapTest, lhmNotFoundAndInsertFindSuccess) {
  auto randomNodeNotInMap = new MapNode(200, new int(200));
  auto randomNodeItr = lhm.find(randomNodeNotInMap->key);
  EXPECT_EQ(randomNodeItr == lhm.end(), true);
  auto [itr, insertRes] = lhm.insert(*randomNodeNotInMap);
  auto randomNodeItrAfter = lhm.find(randomNodeNotInMap->key);
  EXPECT_TRUE(randomNodeItrAfter == itr);
  EXPECT_TRUE(randomNodeItrAfter != lhm.end());
  EXPECT_TRUE(insertRes);
  EXPECT_EQ(lhm.size(), 4);
}

TEST_F(OAHashMapTest, lhmUseOperatorSqBracketToRetrieveValue) {
  auto val = lhm[node1->key];
  EXPECT_EQ(val, *node1->t_p);
  auto val10 = lhm[node10->key];
  EXPECT_EQ(val10, *node10->t_p);
  auto val2 = lhm[node2->key];
  EXPECT_EQ(val2, *node2->t_p);
  auto moveVal = lhm[std::move(node1->key)];
  EXPECT_EQ(moveVal, *node1->t_p);
}

TEST_F(OAHashMapTest, lhmContainsWorks) {
  auto find1 = lhm.contains(node1->key);
  EXPECT_TRUE(find1);
  auto find10 = lhm.contains(node10->key);
  EXPECT_TRUE(find10);
  auto find2 = lhm.contains(node2->key);
  EXPECT_TRUE(find2);
  auto randomNodeNotInMap = MapNode(200, new int(200));
  auto findRandom = lhm.contains(randomNodeNotInMap.key);
  EXPECT_FALSE(findRandom);
}

TEST_F(OAHashMapTest, lhmExpandsWhenLoadFactorIsExceeded) {
  lhm.insert(*(new MapNode(11, new int(31))));
  lhm.insert(*(new MapNode(12, new int(32))));
  lhm.insert(*(new MapNode(13, new int(33))));
  lhm.insert(*(new MapNode(14, new int(34))));
  lhm.insert(*(new MapNode(15, new int(35))));
  lhm.insert(*(new MapNode(16, new int(36))));
  lhm.insert(*(new MapNode(17, new int(37))));
  EXPECT_EQ(lhm.size(), 10);
  EXPECT_GE(lhm.max_size(), 10);
}

TEST_F(OAHashMapTest, lhmClearWorks) {
  EXPECT_EQ(lhm.size(), 3);
  lhm.clear();
  EXPECT_EQ(lhm.size(), 0);
  EXPECT_EQ(lhm.max_size(), 0);
}

TEST_F(OAHashMapTest, lhmReplacesDupKeyValue) {
  // causes a memory leak unless I add a check...
  lhm.insert(*(new MapNode(node1->key, new int(123))));
  lhm.insert(*(new MapNode(node1->key, new int(531))));
  lhm.insert(*(new MapNode(node1->key, new int(52))));
  EXPECT_EQ(lhm.size(), 3); // assert size has not expanded
  auto itr = lhm.find(node1->key);
  EXPECT_EQ(*itr->t_p, 52);
}

TEST_F(OAHashMapTest, lhmThrowsOnUnidentifiedKey) {
  EXPECT_THROW(lhm.at(100), std::out_of_range);
  EXPECT_THROW(lhm.at(15124), std::out_of_range);
}