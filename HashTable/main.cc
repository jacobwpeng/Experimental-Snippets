/*
 * ==============================================================================
 *
 *       Filename:  main.cc
 *        Created:  04/26/15 15:15:04
 *         Author:  Peng Wang
 *          Email:  pw2191195@gmail.com
 *    Description:
 *
 * ==============================================================================
 */

#include <bits/stdc++.h>

template<typename Value>
struct HashTableNode {
  HashTableNode(const Value& val)
    : next(nullptr), v(val) {
  }
  HashTableNode* next;
  Value v;
};

template<typename Key,
         typename Value,
         typename Hash,
         typename KeyOfValue,
         typename Pred>
class HashTable;

template<typename Key,
         typename Value,
         typename Hash,
         typename KeyOfValue,
         typename Pred>
struct HashTableIterator {
  using _HashTableNode = HashTableNode<Value>;
  using _HashTable = HashTable<Key, Value, Hash, KeyOfValue, Pred>;
  HashTableIterator()
    : node(nullptr), ht(nullptr) {
  }
  HashTableIterator(_HashTableNode* node, _HashTable* table)
    : node(node), ht(table) {
  }
  _HashTableNode* node;
  _HashTable* ht;
};

template<typename Key,
         typename Value,
         typename Hash,
         typename KeyOfValue,
         typename Pred>
class HashTable {
 public:
  using key_type = Key;
  using value_type = Value;
  using hasher = Hash;
  using key_equal = Pred;
  using key_of_value = KeyOfValue;
  using hash_node = HashTableNode<value_type>;
  using hash_node_ptr = hash_node* ;
  using iterator =
    HashTableIterator<key_type, value_type, hasher, key_of_value, key_equal>;

  HashTable()
    : buckets_(1 << 20) {
  }

  std::pair<iterator, bool> insert_unique(const value_type& v) {
    const auto& key = key_of_value()(v);
    auto index = BucketIndex(key);
    auto node = Search(index, key);
    bool exists = true;
    if (node == nullptr) {
      exists = false;
      node = new hash_node(v);
      node->next = buckets_[index];
      buckets_[index] = node;
    }
    return std::make_pair(iterator(node, this), !exists);
  }

  size_t erase(const key_type& key) {
    auto index = BucketIndex(key);
    auto node = Search(index, key);
    if (node) {
      auto cur = buckets_[index];
      hash_node_ptr prev = nullptr;
      while (cur != node) {
        prev = cur;
        cur = cur->next;
      }
      if (prev == nullptr) {
        buckets_[index] = node->next;
      } else {
        prev->next = node->next;
      }
      return 1;
    } else {
      return 0;
    }
  }

  iterator find(const key_type& key) {
    auto index = BucketIndex(key);
    return iterator(Search(index, key), this);
  }

  iterator begin() {
    for (size_t i = 0; i < buckets_.size(); ++i) {
      if (buckets_[i]) {
        return iterator(buckets_[i], this);
      }
    }
    return end();
  }

  iterator end() {
    return iterator(nullptr, this);
  }

 private:
  size_t BucketIndex(const key_type& k) const {
    auto h = hasher()(k);
    return h % buckets_.size();
  }

  hash_node_ptr Search(size_t index, const key_type& key) const {
    auto node = buckets_[index];
    while (node) {
      if (key_equal()(key, key_of_value()(node->v))) {
        return node;
      }
      node = node->next;
    }
    return nullptr;
  }
  std::vector<hash_node_ptr> buckets_;
};

template<typename Key, typename Pair>
struct Select1st {
  const Key& operator()(const Pair& p) {
    return p.first;
  }
};

template<typename Key>
struct Identity {
  const Key& operator()(const Key& k) {
    return k;
  }
};

int main() {
  std::unordered_map<int, int> m;
  using Key = int;
  using Value = Key;
  using KeyOfValue = Identity<Key>;
  using HashSet =
    HashTable<Key, Value, std::hash<Key>, KeyOfValue, std::equal_to<Key>>;
  HashSet s;
  auto res = s.insert_unique(5);
  assert (res.second);
  auto p = m.insert(std::make_pair(3, 2));
}
