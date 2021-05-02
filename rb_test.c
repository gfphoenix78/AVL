#include "rbtree.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static inline bool is_red(const struct rb_node *node)
{
  return !node->black;
}
static inline bool is_black(const struct rb_node *node)
{
  return node->black;
}

static struct rb_node *mid_first(struct rb_root *r)
{
  struct rb_node *p;
  if(!r || !r->root)
    return NULL;
  p = r->root;
  while(p->l)
    p = p->l;
  return p;
}
static struct rb_node *mid_next(struct rb_node *x)
{
  struct rb_node *p;
  if(x->r){
    p = x->r;
    while(p->l)
      p = p->l;
    return p;
  }
  while((p = x->p) && p->r == x)
    x = p;
  return p;
}
void mid_iter(struct rb_root *r, void (*fn)(const struct rb_node *,void *a),void *arg)
{
  struct rb_node *x ;
  for(x= mid_first(r); x; x = mid_next(x))
    fn(x,arg);
}
static void check_order(const struct rb_node *node, void *arg)
{
  int *i = arg;
  if (*i < node->val) {
    *i = node->val;
    return;
  }
  // unexpected order
  printf("unexpected order %d - %d\n", *i, node->val);
}

static bool verify_rb_subtree(struct rb_node *node, int *h);
static bool verify_rb_redtree(const struct rb_node *node, int *h)
{
    int h1 = 0;
    int h2 = 0;
    assert(is_red(node));
    if (node->l && !verify_rb_subtree(node->l, &h1))
        return false;
    if (node->r && !verify_rb_subtree(node->r, &h2))
        return false;

    *h = h1;
    if (h1 != h2)
      printf("black height is not equal: val=%d %d %d\n", node->val, h1, h2);
    return h1 == h2;
}
static bool verify_rb_subtree(struct rb_node *node, int *h)
{
    int h1 = 0;
    int h2 = 0;
    assert(is_black(node));
    if (node->l) {
        if (is_black(node->l)) {
            if (!verify_rb_subtree(node->l, &h1))
                return false;
        } else if (is_red(node->l)) {
            if (!verify_rb_redtree(node->l, &h1))
                return false;
        } else
            assert(0);
    }
    if (node->r) {
        if (is_black(node->r)) {
            if (!verify_rb_subtree(node->r, &h2))
                return false;
        } else if (is_red(node->r)) {
            if (!verify_rb_redtree(node->r, &h2))
                return false;
        } else
            assert(0);
    }
    *h = h1 + 1;
    if (h1 != h2)
      printf("black height is not equal: val=%d %d %d\n", node->val, h1, h2);
    return h1 == h2;
}
bool verify_rbtree(struct rb_root *root)
{
    struct rb_node *x = root->root;
    int h = 0;
    int val;
    bool ok = true;
    if (x)
        ok = verify_rb_subtree(x, &h);
    val = -1;
//    mid_iter(root, check_order, &val);
    // print h
    return ok;
}
#define NR  (10*1000)

struct rb_node x[NR];
static int values[NR] = {
};

//#define verify_rbtree(x) true

int main(int ac, const char *av[]) {
  struct rb_root root[1] = { NULL, };
  int i;
  int N = 10;
  bool ok;
  srand(time(0));
repeat:
  for (i = 0; i < NR; i++) {
    int e;
again: 
    x[i].val = rand() % 1000000;

    e = rb_insert(root, x + i);
    if (e == -1)
      goto again;

    ok = verify_rbtree(root);
    if (!ok)
      printf("bad avl\n");
  }
  printf("insert ok\n");

  for (i = 0; i < NR; i++) {
    rb_delete_node(root, &x[i]);
    ok = verify_rbtree(root);
    if(!ok) {
      printf("bad rb = %d\n",i);
      break;
    }
    }
  printf("delete ok\n");
  assert(root[0].root == NULL);

  if (N > 0) {
    N--;
    goto repeat;
  }
  return 0;
}

