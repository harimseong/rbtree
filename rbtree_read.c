#include "rbtree_tools.h"

extern t_rbnode* rb_nil;

t_rbnode*
rb_find(const void* key, const t_rbtree* tree, t_compare cmp)
{
  t_rbnode* cur = tree->root;

  // binary search using user provided cmp function pointer and key
  while (cur != &rb_nil_node) {
    int cmp_ret = cmp(key, cur);
    if (cmp_ret < 0)
      cur = cur->left;
    else if (cmp_ret > 0)
      cur = cur->right;
    else
      return cur;
  }
  return NULL;
}

t_rbnode*
rb_first(t_rbtree* tree)
{
  t_rbnode* node = tree->root;

  if (node == rb_nil) return NULL;

  while (node->left != rb_nil) {
    node = node->left;
  }
  return node;
}

t_rbnode*
rb_next(t_rbnode* node)
{
  if (node->right == &rb_nil_node) {
    t_rbnode* parent = get_parent(node);
    while (parent != &rb_nil_node) {
      if (parent->left == node)
        return parent;
      node = parent;
      parent = get_parent(parent);
    }
    return NULL;
  } else {
    node = node->right;
    while (node->left != &rb_nil_node) {
      node = node->left;
    }
    return node;
  }
}

/*
t_rbnode*  rb_prev(t_rbnode* node)
{
}
*/

// need stack data structure to properly implement this without using recursion.
void
rb_postorder_foreach(t_rbnode* node, void (*op)(t_rbnode*))
{
  if (node == rb_nil) return;

  rb_postorder_foreach(node->left, op);
  node->left = rb_nil;
  rb_postorder_foreach(node->right, op);
  node->right = rb_nil;
  op(node);
}
