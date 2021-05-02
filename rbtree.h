#ifndef _MY_RBTREE_H_
#define _MY_RBTREE_H_
#include <stdbool.h>

struct rb_node {
  struct rb_node *p;  // parent
  struct rb_node *l;   // left child
  struct rb_node *r;  // right child
  bool black;
  int val;
};

struct rb_root {
  struct rb_node *root;
};

extern int rb_insert(struct rb_root *root, struct rb_node *x);
extern void rb_delete_node(struct rb_root *root, struct rb_node *x);

#endif
