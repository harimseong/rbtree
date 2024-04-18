#ifndef RBTREE_DEBUG_H
# define RBTREE_DEBUG_H

#include <stdio.h>
#include <string.h>

#include "rbtree_tools.h"

#define container_of(node, type, member) \
({ \
  void* ptr = (void*)node; \
  (type*)((uintptr_t)ptr - offsetof(type, member)); \
})

// import your container
#define CONTAINER_TYPE t_container
typedef struct container {
  t_rbnode node;
  int       key;
  int       val;
} t_container;

static const int  vis_max_size = 128;
static const int  vis_width = 128;

static char visualize_space[vis_max_size + 1] = {};
static char visualize_bar[vis_max_size + 1] = {};

static bool vis_initialized = false;

static inline void
vis_init(void)
{
  vis_initialized = true;
  memset((void*)visualize_space, ' ', vis_max_size);
  memset((void*)visualize_bar, ' ', vis_max_size);
  for (int i = vis_max_size - 1, width = vis_width; i >= 0; --i, --width) {
    if (width == vis_width - 1) {
      visualize_bar[i] = '|';
      width = vis_width;
    }
  }
}

static inline void
visualize(t_rbnode* root, int depth)
{
  char  node_str[5] = {0, };
  CONTAINER_TYPE* c = container_of(root, CONTAINER_TYPE, node);

  if (vis_initialized == false) vis_init();

  if (root == &rb_nil_node) {
    printf("|-[ NIL]\n");
    return;
  }

  if (get_color(root) == RED) {
    node_str[0] = 'R';
  } else {
    node_str[0] = ' ';
  }
  node_str[1] = c->key / 100 + '0';
  node_str[2] = (c->key / 10) % 10 + '0';
  node_str[3] = c->key % 10 + '0';
  printf("|-[%s]--", node_str);
  ++depth;
  visualize(root->right, depth);
  printf("%s", visualize_bar + vis_max_size - vis_width * depth);
  visualize(root->left, depth);
  printf("\n");
}

static inline void
r4_sanitizer(t_rbnode* root)
{
  if (root == &rb_nil_node)
  {
    if (get_color(&rb_nil_node) == RED) {
      assert(0);
    }
    return;
  }
  if (get_color(root) == RED && (get_color(root->left) == RED || get_color(root->right) == RED))
  {
    assert(0);
  }
  r4_sanitizer(root->left);
  r4_sanitizer(root->right);
}

static inline void
r5_sanitizer_recur(int* max, int* min, int depth, t_rbnode* root)
{
  if (root == &rb_nil_node) {
    if (*max < depth) {
      *max = depth;
    }
    if (*min > depth) {
      *min = depth;
    }
    return;
  }
  r5_sanitizer_recur(max, min, depth + 1, root->left);
  r5_sanitizer_recur(max, min, depth + 1, root->right);
}

static inline int
r5_sanitizer(t_rbnode* root)
{
  int max = 0;
  int min = 2147483647;

  r5_sanitizer_recur(&max, &min, 1, root);
  assert(max <= 2 * min);
  return max;
}

#endif //RBTREE_DEBUG_H
