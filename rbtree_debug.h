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

#define MAX_DIGITS (5)

static const int  vis_max_size = 128;
static const int  vis_width = MAX_DIGITS + 6;

static char visualize_space[vis_max_size + 1] = {};
static char visualize_bar[vis_max_size + 1] = {};

static bool vis_initialized = false;
static int  max_digits_num;
static t_rbnode* leftmost_node;

static inline void
vis_init(void)
{
  vis_initialized = true;
  memset(visualize_bar, ' ', vis_max_size);
  memset(visualize_space, ' ', vis_max_size);
  for (int i = 0; i < vis_max_size; ++i) {
    visualize_bar[i] = (i % vis_width == 0) ? '|' : ' ';
  }
  /*
  for (int i = vis_max_size - 1, width = vis_width - 1; i >= 0; --i, --width) {
    if (width == 0) {
      visualize_bar[i] = '|';
      width = vis_width;
    }
  }
  */
  max_digits_num = 1;
  for (int i = 1; i < MAX_DIGITS; ++i) {
    max_digits_num *= 10;
  }
}

static inline void
visualize(t_rbnode* root, int depth, int left)
{
  char  node_str[MAX_DIGITS + 2] = {0, };
  CONTAINER_TYPE* c = container_of(root, CONTAINER_TYPE, node);
  t_rbnode* parent_leftmost_node = NULL;

  if (vis_initialized == false) vis_init();

  if (depth == 0) {
    leftmost_node = root;
  }

  if (root == &rb_nil_node) {
    printf("|-[%sNIL]\n", visualize_space + vis_max_size - MAX_DIGITS + 2);
    return;
  }

  memset(node_str, ' ', MAX_DIGITS);
  if (get_color(root) == RED) {
    node_str[0] = 'R';
  }
  for (int i = 1, digit = max_digits_num, key = c->key, blank = 1;
       i <= MAX_DIGITS;
       ++i) {

    char c = key / digit + '0';
    if (c != '0') {
      blank = 0;
    }
    node_str[i] = blank ? ' ' : c;
    key %= digit;
    digit /= 10;
  }
  ++depth;
  if (root == leftmost_node) {
    parent_leftmost_node = root;
    leftmost_node = root->left;
  }

  printf("|-[%s]-", node_str);
  visualize_bar[vis_width * (depth - 1)] = left ? ' ' : '|';

  visualize(root->right, depth, 0);

  visualize_bar[vis_width * depth] = '|';
  if (root->right != &rb_nil_node || root->left != &rb_nil_node) {
    visualize_bar[vis_width * depth + 1] = 0;
    printf("%s\n", visualize_bar);
    visualize_bar[vis_width * depth + 1] = ' ';
  }
  visualize_bar[vis_width * depth] = 0;
  printf("%s", visualize_bar);
  visualize_bar[vis_width * depth] = ' ';

  visualize(root->left, depth, 1);

  if (parent_leftmost_node) {
    leftmost_node = parent_leftmost_node;
    printf("\n");
  }
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
