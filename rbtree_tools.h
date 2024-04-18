#ifndef RBTREE_TOOLS_H
# define RBTREE_TOOLS_H

#include <assert.h>

#include "rbtree_types.h"

typedef enum rbtree_color
{
  BLACK = 0, // &rb_nil_node will be black
  RED = 1,
} t_rbcolor;

static const uint8_t rb_left = 0;
static const uint8_t rb_right = 1;

extern t_rbnode rb_nil_node;

static inline t_rbnode*
get_parent(t_rbnode* node)
{
  return (t_rbnode*)((uintptr_t)node->pc.parent & ~1);
}

static inline void
set_parent(t_rbnode* node, t_rbnode* parent)
{
  node->pc.parent = (t_rbnode*)(((uintptr_t)parent & ~1) | node->pc.color);
}

static inline uint8_t 
get_color(const t_rbnode* node)
{
  return node->pc.color;
}

static inline void
set_color(t_rbnode* node, uint8_t color)
{
  assert(!(color == RED && node == &rb_nil_node));
  node->pc.color = color;
}

static inline t_rbnode*
get_far_child(t_rbnode* node, bool left)
{
  return left ? node->right : node->left;
}

static inline void
rotate_nodes(t_rbtree* tree, t_rbnode* parent, bool left)
{
  t_rbnode* c;
  t_rbnode* n;
  t_rbnode* gparent = get_parent(parent);
  bool      pleft = gparent->left == parent;

  /*
   *   g         g
   *    \         \
   *     p         n
   *    / \       / \
   *   n   b =>  a   p
   *  / \           / \
   * a   c         c   b
   */
  // n is left child of p, clockwise
  if (left) {
    n = parent->left;
    c = n->right;
    parent->left = c;
    set_parent(c, parent);
    n->right = parent;
    set_parent(parent, n);
  } else {
  // counter clockwise
    n = parent->right;
    c = n->left;
    parent->right = c;
    set_parent(c, parent);
    n->left = parent;
    set_parent(parent, n);
  }
  if (gparent == &rb_nil_node) {
    tree->root = n;
  } else {
    if (pleft) {
      gparent->left = n;
    } else {
      gparent->right = n;
    }
  }
  set_parent(n, gparent);
}

// create link with the tree and new node
static inline void
link_node(t_rbnode* parent, t_rbnode* node, t_rbnode** insert_at)
{
  // make a edge from new node to parent
  node->pc.parent = parent;
  node->pc.color = RED;
  node->left = &rb_nil_node;
  node->right = &rb_nil_node;

  // insert_at = &parent->left or right
  // make a edge from parent to new node.
  *insert_at = node;
}

static inline void
change_child(t_rbnode* parent, t_rbnode* old, t_rbnode* new_node, t_rbtree* tree)
{
  if (parent != &rb_nil_node) {
    if (parent->left == old)
      parent->left = new_node;
    else
      parent->right = new_node;
  } else
    tree->root = new_node;
}

static inline void
swap_nodes(t_rbnode** n1, t_rbnode** n2)
{
  t_rbnode* n = *n1;
  *n1 = *n2;
  *n2 = n;
}

static inline void
swap_children(t_rbnode* p1, t_rbnode* p2)
{
  set_parent(p1->left, p2);
  set_parent(p1->right, p2);
  set_parent(p2->left, p1);
  set_parent(p2->right, p1);
  swap_nodes(&p1->left, &p2->left);
  swap_nodes(&p1->right, &p2->right);
}

static inline void
swap_edges(t_rbnode* p1, t_rbnode* p2, t_rbnode* n1, t_rbnode* n2, t_rbtree* tree)
{
  uint8_t color = get_color(n1);

  set_color(n1, get_color(n2));
  set_color(n2, color);
  if (p2 == n1) {
    change_child(p1, n1, n2, tree);
    set_parent(n2, p1);
    set_parent(n1, n2);
    if (n1->left == n2) {
      set_parent(n2->left, n1);
      set_parent(n2->right, n1);
      set_parent(n1->right, n2);
      n1->left = n2->left; 
      n2->left = n1;
      swap_nodes(&n1->right, &n2->right);
    } else {
      set_parent(n2->left, n1);
      set_parent(n2->right, n1);
      set_parent(n1->left, n2);
      n1->right = n2->right; 
      n2->right = n1;
      swap_nodes(&n1->left, &n2->left);
    }
    return;
  }
  else if (p1 == n2) {
    change_child(p2, n2, n1, tree);
    set_parent(n1, p2);
    set_parent(n2, n1);
    if (n2->left == n1) {
      set_parent(n1->left, n2);
      set_parent(n1->right, n2);
      set_parent(n2->right, n1);
      n1->left = n2;
      n2->left = n1->left; 
      swap_nodes(&n2->right, &n1->right);
    } else {
      set_parent(n1->right, n2);
      set_parent(n1->left, n2);
      set_parent(n2->left, n1);
      n1->right = n2;
      n2->right = n1->right; 
      swap_nodes(&n2->left, &n1->left);
    }
    return;
  }
  change_child(p1, n1, n2, tree);
  set_parent(n2, p1);
  change_child(p2, n2, n1, tree);
  set_parent(n1, p2);
  swap_children(n1, n2);
}

#endif // RBTREE_TOOLS_H
