# RB tree
This is an RB tree implementation that I used to implement Completely Fair Scheduler and integrate it into xv6.

## Linux Reference
Interface and some design choices are inspired by [the Linux RB tree](https://github.com/torvalds/linux/blob/master/lib/rbtree.c).
- Leftmost node caching and inline functions in the header.
- `container_of` and `offsetof` macros.
- Using first bit of the parent node's address to represent color, assuming addresses are 8 bytes aligned.

Additionally, the color bit is easily accesible using a union.\
`t_rbnode` struct in `rbtree_type.h`
```
typedef struct rbnode
{
    union share {
        struct rbnode*  parent;
        bool            color: 1;
    } pc;
    struct rbnode*  right;
    struct rbnode*  left;
} t_rbnode;
```

## Visualize
The `visualize` function can be inserted anywhere to visualize the tree.\
Using it at the end of `rb_insert` and `rb_erase` allows the user to track changes. 

- example
```
|-[  4850]-|-[  9507]-|-[   NIL]
           |          |
           |          |-[R 8136]-|-[   NIL]
           |                     |-[   NIL]
           |
           |-[  1924]-|-[   NIL]
                      |-[   NIL]
```
Insert a node with the key 4026
```
|-[  4850]-|-[  9507]-|-[   NIL]
           |          |
           |          |-[R 8136]-|-[   NIL]
           |                     |-[   NIL]
           |
           |-[  1924]-|-[R 4026]-|-[   NIL]
                      |          |-[   NIL]
                      |
                      |-[   NIL]
```
Erase the leftmost node with the key 1924
```
|-[  4850]-|-[  9507]-|-[   NIL]
           |          |
           |          |-[R 8136]-|-[   NIL]
           |                     |-[   NIL]
           |
           |-[  4026]-|-[   NIL]
                      |-[   NIL]
```

## Debug
`rbtree_debug.h` provides functions to test the validity of the RB tree.
- `r4_sanitizer`


Checks **Rule 4**: if a node is red, its children must be black.\
This test ensures that the balancing logic is working correctly.
- `r5_sanitizer`


Verifies the length of longest path is not greater than twice the length of shortest path and asserts if the condition is violated.\
This property is crucial for RB tree performance, as logarithmic time complexity depends on it.
