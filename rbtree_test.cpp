#include <iostream>
#include <map>
#include <unordered_set>
#include <vector>
#include <chrono>

#include <ctime>
#include <cstdlib>
#include <string.h>
#include <assert.h>

#include "rbtree.h"

static bool       compare_flag = true;

typedef struct container {
  t_rbnode  node;
  int       key;
  int       val;
} t_container;

void deallocate(t_rbnode* node);

bool  rb_less(t_rbnode* n1, const t_rbnode* n2)
{
  t_container*  c1 = container_of(n1, t_container, node);
  t_container*  c2 = container_of(n2, t_container, node);

  return c1->key < c2->key;
}

void  rb_swap(t_rbnode* n1, t_rbnode* n2)
{
  t_container*  c1 = container_of(n1, t_container, node);
  t_container*  c2 = container_of(n2, t_container, node);
  t_rbnode     n;

  n = c1->node;
  c1->node = c2->node;
  c2->node = n;
}

int   rb_compare(const void* key, const t_rbnode* node)
{
  const long int k = reinterpret_cast<long int>(key);
  t_container*  c = container_of(node, t_container, node);

  return k - c->key;
}

bool  check_equal(t_rbtree* tree, std::map<int, int>& map)
{
  int       itr_count = 0;
//  t_rbnode* node = rb_first(tree);
  t_rbnode* node = rb_first(tree);
  if (node == NULL && map.size() == 0) return true;
  //t_container*  c0 = container_of(node, t_container, node);
  //std::cout << "first node\n";
  //std::cout << "c->node = " << &c0->node << ", c->key = " << c0->key << ", c->val = " << c0->val << '\n';

  for (std::map<int, int>::iterator itr = map.begin(); itr != map.end(); ++itr, ++itr_count) {
    if (node == NULL) {
      std::cout << "count=" << itr_count << ":node is nil\n";
      assert(0);
      return false;
    }
    t_container*  c = container_of(node, t_container, node);
    if (itr->first != c->key) {
      std::cout << "failed at " << itr_count << '\n';
      std::cout << "itr->first = " << itr->first << '\n';
      std::cout << "c->node = " << &c->node << ", c->key = " << c->key << ", c->val = " << c->val << '\n';
      return false;
    }
    node = rb_next(node);
  }
  return node == NULL;
}

void check_erase(std::vector<int>& idx_arr, rbtree* rbtree, std::map<int, int>& map)
{
  for (size_t i = 0; i < idx_arr.size(); ++i) {
    int key = idx_arr[i];

    //std::cout << std::endl;
    //std::cout << "key = " << key << ", c->node = " << &c->node << ", c->key = " << c->key << ", c->val = " << c->val << '\n';
    t_rbnode* node = rb_find(reinterpret_cast<void*>(key), rbtree, rb_compare);
    t_container* c = container_of(node, t_container, node);
    rb_erase(rbtree, node);
    free(c);
    if (compare_flag) {
      map.erase(key);
    }

    /*
    if (check_equal(rbtree, map) == false) {
      std::cout << i << "th erase, key = " << key << ": check_erase fail\n";
      return;
    }
    */
  }
  //std::cout << "erase success\n";
}

void deallocate(t_rbtree* tree)
{
  t_rbnode* node = rb_first(tree);

  while (node != NULL) {
    t_container* temp = container_of(node, t_container, node);

    // it must be post-order
    node = rb_next(node);
    free(temp);
  }
}

int main(int argc, char **argv)
{
  // number of test cases
  int test_count  = 10;

  // initial number of elements for a test. doubled per test completion.
  int max_size = 1024;

  // number of loops to get average time consumption.
  int repeat_count = 1;
  if (argc > 1) {
    test_count  = 1;
    max_size = 16;
    repeat_count = 1;
  }

  int dup_count = 0;
  t_rbtree_cached         rbtree = rb_create_tree_cached();
  std::map<int, int>      map;
  std::unordered_set<int> number_table;


  std::srand(static_cast<uintptr_t>(clock()));

  for (int test_idx = 0; test_idx < test_count; ++test_idx) {
    double    rb_insert_time = 0;
    double    map_insert_time = 0;
    //double    e_duration_avg = 0;

    std::vector<int> idx_arr(max_size);
    for (int repeat = 0; repeat < repeat_count; ++repeat) {
      if (rbtree.rbtree.root != rb_nil || map.size() != 0) assert(0);
      for (int i = 0; i < max_size; ++i) {
        int random_range = max_size * 3;
        int key = std::rand() % random_range;
        if (number_table.count(key) != 0) {
          ++dup_count;
          --i;
          continue;
        }
        number_table.insert(key);
        idx_arr[i] = key;
      }

      auto start = std::chrono::steady_clock::now();
      for (int i = 0; i < max_size; ++i) {
        int key = idx_arr[i];
        
        t_container* container;
        container = static_cast<t_container*>(malloc(sizeof(t_container)));
        *container = (t_container){.key = key, .val = key * key};
        rb_insert_cached(&rbtree, &container->node, rb_less);
      }
      auto end = std::chrono::steady_clock::now();
      std::chrono::duration<double>  i_duration(end - start);
      rb_insert_time += i_duration.count();

      if (compare_flag) {
        start = std::chrono::steady_clock::now();
        for (int i = 0; i < max_size; ++i) {
          int key = idx_arr[i];
          map.insert(std::make_pair(key, key * key));
        }
        end = std::chrono::steady_clock::now();
        std::chrono::duration<double>  i_duration(end - start);
        map_insert_time += i_duration.count();
      }

      if (compare_flag && check_equal(&rbtree.rbtree, map) == false) {
        std::cout << "case " << test_idx << ": fail\n";
        break;
      }
      check_erase(idx_arr, &rbtree.rbtree, map);
      /*
      std::cout << "insert time = " << i_duration.count() << '\n';
      auto start_erase = std::chrono::steady_clock::now();
      auto end_erase = std::chrono::steady_clock::now();
      std::chrono::duration<double>  e_duration(end_erase - start_erase);
      std::cout << "erase time = " << e_duration.count() << "\n\n";
      e_duration_avg += e_duration.count();
      */
    }
    std::cout << "case " << test_idx << ": data size=" << max_size << '\n';
    std::cout << "rbtree = " << rb_insert_time / repeat_count;
    if (compare_flag) {
      std::cout << ", map = " << map_insert_time / repeat_count;
      std::cout << ", ratio = " << rb_insert_time / map_insert_time << '\n';
    }
    std::cout << '\n';
    max_size *= 2;
  }
}

void deallocate(t_rbnode* node)
{
  t_container*  c = container_of(node, t_container, node);

	node->left = rb_nil;
	node->right = rb_nil;
  free(c);
}
