#include <iostream>
#include <map>
#include <vector>
#include <chrono>

#include <ctime>
#include <cstdlib>
#include <string.h>
#include <assert.h>

#include "rbtree.h"

static const int  random_range = 100000;
static int        max_size;
static bool       compare_flag = false;

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
    t_rbnode* node = rb_find(reinterpret_cast<void*>(key), rbtree, rb_compare);
    t_container* c = container_of(node, t_container, node);

    //std::cout << std::endl;
    //std::cout << "key = " << key << ", c->node = " << &c->node << ", c->key = " << c->key << ", c->val = " << c->val << '\n';
    if (compare_flag) {
      rb_erase(rbtree, node);
    } else {
      map.erase(key);
    }

    /*
    free(c);
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
  int dup_count = 0;
  int arr[random_range] = {0, };
  t_rbtree_cached   rbtree = rb_create_tree_cached();
  std::map<int, int>  map;
  std::vector<double> rbtree_insert_time;
  std::vector<double> rbtree_erase_time;
  std::vector<double> stdmap_insert_time;
  std::vector<double> stdmap_erase_time;
//  t_rbtree   rbtree = rb_create_tree();

  std::srand(static_cast<uintptr_t>(clock()));

  compare_flag = true;
  while (true) {
    const int test_count  = 10; // size up to max_size(==1024) * 2^8 
    std::vector<int>    test_size;
    max_size = 128;
    for (int test_idx = 0; test_idx < test_count; ++test_idx) {
      const int repeat_count = 6;
      double    i_duration_avg = 0;
      double    e_duration_avg = 0;

      test_size.push_back(max_size);
      std::vector<int> idx_arr(max_size);
      for (int repeat = 0; repeat < repeat_count; ++repeat) {
        memset(arr, 0, sizeof(arr));
        if (rbtree.rbtree.root != rb_nil || map.size() != 0) assert(0);
        for (int i = 0; i < max_size; ++i) {
          int key = std::rand() % random_range;
          if (arr[key] != 0) {
            ++dup_count;
            --i;
            continue;
          }
          arr[key] = 1;
          idx_arr[i] = key;
        }

        auto start = std::chrono::steady_clock::now();
        for (int i = 0; i < max_size; ++i) {
          t_container* container = static_cast<t_container*>(malloc(sizeof(t_container)));
          int key = idx_arr[i];

          container->key = key;
          container->val = key;

          if (compare_flag) {
            rb_insert_cached(&rbtree, &container->node, rb_less);
          } else {
            map.insert(std::make_pair(key, key));
          }
        }
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<double>  i_duration(end - start);
        /*
        if (check_equal(&rbtree.rbtree, map) == false) {
          std::cout << "fail\n";
        } else {
          std::cout << "insert success\n";
        }
        */
        //std::cout << "insert time = " << i_duration.count() << '\n';
        auto start_erase = std::chrono::steady_clock::now();
        check_erase(idx_arr, &rbtree.rbtree, map);
        auto end_erase = std::chrono::steady_clock::now();
        std::chrono::duration<double>  e_duration(end_erase - start_erase);
        //std::cout << "erase time = " << e_duration.count() << "\n\n";
        rb_postorder_foreach(rbtree.rbtree.root, deallocate);
        i_duration_avg += i_duration.count();
        e_duration_avg += e_duration.count();
      }
      if (compare_flag) {
        rbtree_insert_time.push_back(i_duration_avg / repeat_count);
        rbtree_erase_time.push_back(e_duration_avg / repeat_count);
      } else {
        stdmap_insert_time.push_back(i_duration_avg / repeat_count);
        stdmap_erase_time.push_back(e_duration_avg / repeat_count);
      }
      max_size *= 2;
    }
    if (compare_flag) {
      std::cout << "rbtree profile\n";
      for (int i = 0; i < test_count; ++i) {
        std::cout << "case " << i << ": data size=" << test_size[i] << ", average insertion time=" << rbtree_insert_time[i] << ", average erase time=" << rbtree_erase_time[i] << '\n';
      }
    } else {
      std::cout << "std::map profile\n";
      for (int i = 0; i < test_count; ++i) {
        std::cout << "case " << i << ": data size=" << test_size[i] << ", average insertion time=" << stdmap_insert_time[i] << ", average erase time=" << stdmap_erase_time[i] << '\n';
      }
      std::cout << "runtime ratio\n";
      for (int i = 0; i < test_count; ++i) {
        std::cout << "case " << i << ": data size=" << test_size[i] << ", std::map and rbtree runtime ratio(stdmap/rbtree): insert=" << stdmap_insert_time[i] / rbtree_insert_time[i] << ", erase=" << stdmap_erase_time[i] / rbtree_erase_time[i] << '\n';
      }
    }
    if (compare_flag == false) break;
    compare_flag = false;
  }
}

void deallocate(t_rbnode* node)
{
  t_container*  c = container_of(node, t_container, node);

  free(c);
}
