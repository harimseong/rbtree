/*
 * rbtree.h
 *
 * Red-black tree interface
 * referenced Linux rbtree implementation
 */

#ifndef RBTREE_H
# define RBTREE_H

#include <stddef.h> // offsetof

#include "rbtree_types.h"

#define container_of(node, type, member) \
({ \
  void* ptr = (void*)node; \
  (type*)((uintptr_t)ptr - offsetof(type, member)); \
})

extern t_rbnode*  rb_find(const void* key, const t_rbtree* tree, t_compare cmp);
extern t_rbnode*  rb_next(t_rbnode* node);
extern t_rbnode*  rb_first(t_rbtree* tree);
// extern t_rbnode*  rb_prev(t_rbnode* node);

extern void rb_postorder_foreach(t_rbnode* node, void (*op)(t_rbnode*));

extern void rb_insert(t_rbtree* tree, t_rbnode* node, t_less less);
extern void rb_insert_cached(t_rbtree_cached* tree, t_rbnode* node, t_less less);
extern void rb_erase(t_rbtree* tree, t_rbnode* node);

extern t_rbnode* rb_nil;

static inline t_rbtree
rb_create_tree(void)
{
  return (t_rbtree){.root = rb_nil};
}

static inline t_rbtree_cached
rb_create_tree_cached(void)
{
  return (t_rbtree_cached){
    .rbtree = rb_create_tree(),
    .leftmost_node = rb_nil,
  };
}

static inline void
rb_erase_cached(t_rbtree_cached* tree, t_rbnode* node)
{
  if (tree->leftmost_node == node) {
    tree->leftmost_node = rb_next(node);
  }
  rb_erase(&tree->rbtree, node);
}

static inline t_rbnode*
rb_find_cached(const void* key, const t_rbtree_cached* tree, t_compare cmp)
{
  if (cmp(key, tree->leftmost_node)) {
    return tree->leftmost_node;
  }
  return rb_find(key, &tree->rbtree, cmp);
}

static inline t_rbnode*
rb_leftmost(t_rbtree_cached* tree)
{
  return tree->leftmost_node;
}

static inline bool
rb_is_nil(t_rbnode* node)
{
  return node == rb_nil;
}

#endif // RBTREE_H
