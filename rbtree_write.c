/*
 * rbtree_write.c
 *
 * Red-black tree implementation for node insert and erase
 *
 * reference
 * https://en.wikipedia.org/wiki/Red-black_tree
 * https://elixir.bootlin.com/linux/latest/source/lib/rbtree.c
 */
#include "rbtree_tools.h"

#ifdef RB_DEBUG
# include "rbtree_debug.h"
# define DEBUG_FUNCTIONS(tree) \
  r4_sanitizer((tree)->root); \
  r5_sanitizer((tree)->root);
#else
# define DEBUG_FUNCTIONS(tree)
#endif

#ifdef RB_VIS
# include "rbtree_debug.h"
# define VISUALIZE(tree) \
  visualize((tree)->root, 0, 1);
#else
# define VISUALIZE(tree)
#endif

static void insert_balance(t_rbtree* tree, t_rbnode* node);
static void erase_balance(t_rbtree* tree, t_rbnode* node);

t_rbnode  rb_nil_node = {
  {&rb_nil_node}, &rb_nil_node, &rb_nil_node,
};
t_rbnode* rb_nil = &rb_nil_node;
/*
 * Red-black tree requirements
 * 1. All nodes are either red or black.
 * 2. The root node is black.
 * 3. NIL node is black.
 * 4. If a node is red, its children nodes are black.
 * 5. Arbitrary two simple paths from root node to leaf node have same number of black nodes on their path.
 * (conclusion) If a child node have no other sibling, its color must be red.
 */

// section 1. insert

/*
 * @tree: red-black tree to insert in
 * @node: new node
 * @less: user provided boolean function which indicate less or not
 */
void
rb_insert(t_rbtree* tree, t_rbnode* node, t_less less)
{
  t_rbnode**  insert_at = &tree->root;
  t_rbnode*   parent = rb_nil;

  // binary search to find place to insert
  while (*insert_at != rb_nil) {
    bool less_ret;

    parent = *insert_at;
    less_ret = less(node, parent);
    if (less_ret)
      insert_at = &parent->left;
    else
      insert_at = &parent->right;
  }
  // create bidirectional edge between parent and new node
  link_node(parent, node, insert_at);

  // set color of inserted node red and rebalance if required.
  insert_balance(tree, node);
  DEBUG_FUNCTIONS(tree);
  VISUALIZE(tree);
}

void
rb_insert_cached(t_rbtree_cached* tree, t_rbnode* node, t_less less)
{
  t_rbnode**  insert_at = &tree->rbtree.root;
  t_rbnode*   parent = rb_nil;
  bool        leftmost = false;

  while (*insert_at != rb_nil) {
    bool  less_ret;

    parent = *insert_at;
    less_ret = less(node, parent);
    if (less_ret)
      insert_at = &parent->left;
    else {
      insert_at = &parent->right;
      less_ret = true;
    }
  }
  // if the node to be inserted is leftmost, cache it
  if (leftmost)
    tree->leftmost_node = node;

  link_node(parent, node, insert_at);
  insert_balance(&tree->rbtree, node);
  DEBUG_FUNCTIONS(&tree->rbtree);
  VISUALIZE(&tree->rbtree);
}

static void
insert_balance(t_rbtree* tree, t_rbnode* node)
{
  // loop for recursion
  while (true) {
    t_rbnode* parent = get_parent(node);

    // case 1.
    // previous root of the tree was nil and new node is root.
    if (parent == rb_nil) {
      node->pc.color = BLACK;
      return;
    }

    // case 2. 
    // parent of inserted node is black.
    if (get_color(parent) == BLACK) {
      return;
    }

    // case 3.
    // uncle node is red.

    // grand parent exists and its color is black because parent node is red.
    t_rbnode* gparent;
    t_rbnode* uncle;
    bool      u_left = false;

    gparent = get_parent(parent);
    if (gparent->left == parent) {
      uncle = gparent->right;
    } else {
      u_left = true;
      uncle = gparent->left;
    }
    if (get_color(uncle) == RED) {
      set_color(parent, BLACK);
      set_color(uncle, BLACK);
      set_color(gparent, RED);
      node = gparent;
      // recursive balancing as if gparent is inserted node.
      continue;
    }

    // case 4.
    // uncle node is black and has same branch direction with new node
    bool  n_right = node == parent->right;

    if (n_right ^ u_left) {
      rotate_nodes(tree, parent, !n_right);
      swap_nodes(&node, &parent);
    }

    // case 5.
    // uncle node is black and has different branch direction with new node
    rotate_nodes(tree, gparent, !u_left);
    set_color(parent, BLACK);
    set_color(gparent, RED);
    return;
  }
}

// section 2. erase

/*
 * @tree: red-black tree to erase from
 * @node: node to remove
 */
void
rb_erase(t_rbtree* tree, t_rbnode* node)
{
  t_rbnode* parent = get_parent(node);


  if (node == rb_nil) goto ret;

  // case 1.
  // node has two children
  // let successor of the node be leftmost node of right sub-tree.
  // successor has either no child node or right child node.
  if (node->left != rb_nil && node->right != rb_nil) {
    t_rbnode* successor = node->right;
    while (successor->left != rb_nil) {
      successor = successor->left;
    }
    swap_edges(parent, get_parent(successor), node, successor, tree);
    parent = get_parent(node);
    // node and succesor is swapped. node has either no child or one child which will be handled later.
  }

  // case 2.
  // node has one child which is red.
  if (node->right != rb_nil) {
    t_rbnode* right = node->right;
    change_child(parent, node, right, tree);
    set_parent(right, parent);
    set_color(right, BLACK);
    goto ret; // no requirements are violated.
  } else if (node->left != rb_nil) {
    t_rbnode* left = node->left;
    change_child(parent, node, left, tree);
    set_parent(left, parent);
    set_color(left, BLACK);
    goto ret;
  }

  // case 3.
  // node has no child and is root.
  if (parent == rb_nil) {
    change_child(rb_nil, rb_nil, rb_nil, tree);
    goto ret;
  }

  // case 4.
  // node has no child and is red.
  if (get_color(node) == RED) {
    change_child(parent, node, rb_nil, tree);
    goto ret;
  }

  // case 5.
  // node has no child and is black.
  change_child(parent, node, rb_nil, tree);
  erase_balance(tree, parent);

ret:
  DEBUG_FUNCTIONS(tree);
  VISUALIZE(tree);
}

/*
 * handles erasing black node with no child.
 */
static void
erase_balance(t_rbtree* tree, t_rbnode* parent)
{
  t_rbnode* node = rb_nil;
  t_rbnode* sibling;
  t_rbnode* close_nephew;
  t_rbnode* far_nephew;
  bool      left;

  while (true) {
    // case 1.
    // Node is root. -1 black height on every path.
    if (parent == rb_nil) {
      return;
    }

    if (node == parent->left) {
      left = true;
      sibling = parent->right;
      close_nephew = sibling->left;
      far_nephew = sibling->right;
    } else {
      left = false;
      sibling = parent->left;
      close_nephew = sibling->right;
      far_nephew = sibling->left;
    }

    // case 2.
    // S is red and other nodes are black.
    // as a result no change on black height, the node and parent, but with different S, C, F positions.
    if (get_color(sibling) == RED) {
      rotate_nodes(tree, parent, !left);
      set_color(sibling, BLACK);
      set_color(parent, RED);
      // sibling has changed. set affected nodes accordingly.
      if (left) {
        sibling = parent->right;
        close_nephew = sibling->left;
        far_nephew = sibling->right;
      } else {
        sibling = parent->left;
        close_nephew = sibling->right;
        far_nephew = sibling->left;
      }
    }

case_3:
    // case 3.
    // F is red. sibling will inherit color of parent.
    if (far_nephew != rb_nil && get_color(far_nephew) == RED) {
      rotate_nodes(tree, parent, !left);
      set_color(sibling, get_color(parent));
      set_color(parent, BLACK);
      set_color(far_nephew, BLACK);
      return;
    }

    // case 4.
    // C is red and F is black. sibling will inherit color of parent.
    if (close_nephew != rb_nil && get_color(close_nephew) == RED) {
      rotate_nodes(tree, sibling, left);
      set_color(close_nephew, BLACK);
      set_color(sibling, RED);

      // adjust nodes after rotation
      far_nephew = sibling;
      sibling = close_nephew;
      close_nephew = left ? close_nephew->left : close_nephew->right;
      goto case_3;
    }

    // case 5.
    // P, S, C and F are black.
    // one less black height on node side. requirement 5 is violated.
    // change color of S as red and set P as new node and balance recursively.
    // first iteration: the node is nil, other iterations: the node is root of subtree which has one less black height.
    if (get_color(parent) == BLACK) {
      set_color(sibling, RED);
      node = parent;
      parent = get_parent(parent);
      continue;
    }
    // case 6.
    // P is red and other nodes are black.
    // changing color of P and S resolves requirement5 violation.
    else {
      set_color(parent, BLACK);
      set_color(sibling, RED);
      return;
    }
  }
}
