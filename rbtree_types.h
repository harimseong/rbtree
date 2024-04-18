#ifndef RBTREE_TYPES_H
# define RBTREE_TYPES_H

#include <stdint.h>
#include <stdbool.h>  // bool
#include <stddef.h>   // NULL

typedef struct rbnode
{
  union share {
    struct rbnode*  parent;
    bool            color: 1;
  } pc;
  struct rbnode*  right;
  struct rbnode*  left;
} t_rbnode;

typedef struct rbtree
{
  t_rbnode*  root;
} t_rbtree;

typedef struct rbtree_cached
{
  t_rbtree  rbtree;
  t_rbnode* leftmost_node;
} t_rbtree_cached;

typedef int     (*t_compare)(const void*, const t_rbnode*);
typedef bool    (*t_less)(t_rbnode*, const t_rbnode*);
typedef void    (*t_swap)(t_rbnode*, t_rbnode*);

#endif // RBTREE_TYPES_H
