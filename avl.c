#include <assert.h>
#include <stdbool.h> /// only for type bool, true and false
#include "avl.h"

static inline void __link_parent_to_child(struct avl_root *root, struct avl_node *x, struct avl_node *c)
{
  struct avl_node *parent = x->p;
  if (parent) {
    if (parent->l == x)
      parent->l = c;
    else
      parent->r = c;
  } else
    root->root = c;
}

static inline void __left_rotate(struct avl_node *n, struct avl_root *root)
{
  struct avl_node *parent = n->p;
  struct avl_node *right = n->r;
  if ((n->r = right->l))
    n->r->p = n;

  __link_parent_to_child(root, n, right);
  right->p = parent;
  n->p = right;
  right->l = n;
}
static inline void __right_rotate(struct avl_node *n, struct avl_root *root)
{
  struct avl_node *parent = n->p;
  struct avl_node *left = n->l;
  if ((n->l = left->r))
    n->l->p = n;

  __link_parent_to_child(root, n, left);
  left->p = parent;
  n->p = left;
  left->r = n;
}

static inline void avl_lr_rotate(struct avl_node *parent, struct avl_root *r)
{
  __left_rotate(parent->l, r);
  __right_rotate(parent, r);
}
static inline void avl_rl_rotate(struct avl_node *parent, struct avl_root *r)
{
  __right_rotate(parent->r, r);
  __left_rotate(parent, r);
}

static void avl_fixup_insert(struct avl_root *r, struct avl_node *x)
{
  struct avl_node *parent;
  struct avl_node *child;

  for (; (parent = x->p); x = parent){
    if (parent->l == x) {
      switch (++parent->balance) {
        case 0: return;
        case 1: continue;
        default: assert(parent->balance == 2);
      }
      //rotate
      if (x->balance == 1) { //LL
        x->balance = parent->balance = 0;
        __right_rotate(parent, r);
        break;
      }
      //LR, x->balance == -1
      assert(x->balance == -1);
      child = x->r;
      switch (child->balance) {
        case +1:
          child->balance = x->balance = 0;
          parent->balance = -1;
          break;
        case -1:
          child->balance = parent->balance = 0;
          x->balance = +1;
          break;
        case 0:
          x->balance = parent->balance = 0;
          assert(child->l == 0);
          assert(child->r == 0);
          break;
        default:
          assert(0);
      }

      avl_lr_rotate(parent, r);
      break;
    } else {
      switch (--parent->balance) {
        case 0: return;
        case -1: continue;
        default: assert(parent->balance == -2);
      }
      //rotate
      if (x->balance == -1) {
        x->balance = parent->balance = 0;
        __left_rotate(parent, r);
        break;
      }
      //RL, x->balance = 1
      assert(x->balance = 1);
      child = x->l;
      switch (child->balance) {
        case -1:
          child->balance = x->balance = 0;
          parent->balance = +1;
          break;
        case +1:
          child->balance = parent->balance = 0;
          x->balance = -1;
          break;
        case 0:
          x->balance = parent->balance = 0;
          assert(child->l == 0);
          assert(child->r == 0);
          break;
        default:
          assert(0);
      }

      avl_rl_rotate(parent, r);
      break;
    }
  }
}


// x's balance is to be decreased
// left == true child is left, left == false, child is right
static void avl_fixup_del(struct avl_root *r, struct avl_node *x, bool left)
{
  struct avl_node *parent;
  struct avl_node *child;
  struct avl_node *gc;
  for (; x; x = parent){
    parent = x->p;
    if (left) {
      x->balance--;
      if (x->balance == -1)
        break;
      if (parent && parent->r == x)
        left = false;
      if (x->balance == 0)
        continue;

      child = x->r;
      assert(x->balance == -2 && child);
      if (child->balance == 0) {
        x->balance = -1;
        child->balance = 1;
        __left_rotate(x, r);
        break;
      }
      if (child->balance == -1) {
        x->balance = child->balance = 0;
        __left_rotate(x, r);
        continue;
      }

      gc = child->l;
      assert(child->balance == 1 && gc);
      switch (gc->balance) {
        case -1:
          child->balance = gc->balance = 0;
          x->balance = +1;
          break;
        case 0:
          child->balance = x->balance = 0;
          break;
        case +1:
          gc->balance = x->balance = 0;
          child->balance = -1;
          break;
        default:
          assert(0);
      }
      avl_rl_rotate(x, r);
    } else {
      x->balance++;
      if (x->balance == 1)
        break;
      if (parent && parent->l == x)
        left = true;
      if (x->balance == 0)
        continue;

      child = x->l;
      assert(x->balance == 2 && child);
      if (child->balance == 0) {
        x->balance = 1;
        child->balance = -1;
        __right_rotate(x, r);
        break;
      }
      if (child->balance == 1) {
        x->balance = child->balance = 0;
        __right_rotate(x, r);
        continue;
      }

      gc = child->r;
      assert(child->balance == -1 && gc);
      switch (gc->balance) {
        case +1:
          child->balance = gc->balance = 0;
          x->balance = -1;
          break;
        case 0:
          child->balance = x->balance = 0;
          break;
        case -1:
          gc->balance = x->balance = 0;
          child->balance = +1;
          break;
        default:
          assert(0);
      }
      avl_lr_rotate(x, r);
    }
  }
}

struct avl_node *avl_search(struct avl_root *r, int key)
{
  struct avl_node *n = r->root;
  while (n) {
    if (n->val < key)
      n = n->r;
    else if (key < n->val)
      n = n->l;
    else
      return n;
  }
  return 0;
}

//
int avl_insert(struct avl_root *root, struct avl_node *x)
{
  struct avl_node *parent = 0;
  struct avl_node **n = &root->root;
  x->l = x->r = 0;
  x->balance = 0;

  while ((*n)) {
    parent = *n;
    if (parent->val < x->val)
      n = &parent->r;
    else if (parent->val > x->val)
      n = &parent->l;
    else
      return -1;
  }
  *n = x;
  x->p = parent;
  avl_fixup_insert(root, x);
  return 0;
}


struct avl_node *avl_delete(struct avl_root *r, int val)
{
  struct avl_node *n;
  n = avl_search(r, val);
  if (!n)
    return 0;
  avl_del_node(r, n);
  return n;
}
#if 0
static inline struct avl_node *_avl_del_prev(struct avl_node *x)
{
  x = x->l;
  while (x->r)
    x = x->r;
  return x;
}
static inline struct avl_node *_avl_del_next(struct avl_node *x)
{
  x = x->r;
  while (x->l)
    x = x->l;
  return x;
}

bool avl_find_replacement(struct avl_root *r, struct avl_node *n, struct avl_node **target)
{

  struct avl_node *parent;
  struct avl_node *child;
  struct avl_node *target;
  struct avl_node *x, *xp;
  *target = parent = n->p;
  if (!n->l) {
    if (parent) {
      if (parent->l == n)
        parent->l = n->r;
      else if (parent->r == n)
        parent->r = n->r;
    } else
      r->root = n->r;
    if (n->r)
      n->r->p = parent;
  } else if (!n->r) {
    if (parent) {
      if (parent->l == n)
        parent->l = n->l;
      else if (parent->r == n)
        parent->r = n->l;
      else
        assert(0);
    } else 
      r->root = n->l;
    if (n->l)
      n->l->p = parent;
  } else {
    // n->l != nil && n->r != nil
    switch (n->balance) {
      case -1:
      x = _avl_del_next(n);
      child = x->l;
      break;
      case +1:
      x = _avl_del_prev(n);
    child = x->r;
      break;
      case 0:
      x = _avl_del_next(n);
      if (x->p && x->p->balance != 0)
        x = _avl_del_prev(n);
      break;
      default:
      assert(0);
  }
    xp = x->p;
    child = x->l + x->p;
    
  }
}
void avl_del_node2(struct avl_root *r, struct avl_node *n)
{
  struct avl_node *parent;
  struct avl_node *child;
  struct avl_node *target;
  struct avl_node *x, *xp;
  bool left;
  target = parent = n->p;
  if (!n->l) {
  }
  switch (n->balance) {
    case -1:
      break;
    case 0:
      break;
    case +1:
      break;
    default:
      assert(0);
  }

}
#endif

void avl_del_node(struct avl_root *r, struct avl_node *n)
{
  struct avl_node *parent;
  struct avl_node *child;
  struct avl_node *target;
  bool left;
  target = parent = n->p;
  if (!n->l)
    child = n->r;
  else if (!n->r)
    child = n->l;
  else {
    //both n->l and n->r are not nil
    struct avl_node *x = n->r;
    struct avl_node *xp;
    while (x->l)
      x = x->l;

    child = x->r;
    xp = x->p;
    // parent link to new child
    if (parent) {
      if (parent->l == n)
        parent->l = x;
      else
        parent->r = x;
    } else
      r->root = x;
    // replace `n` by `x`
    x->p = parent; //
    x->l = n->l;
    n->l->p = x;
    x->balance = n->balance;

    if (xp != n) { //n->r has no left child
      if (child)
        child->p = xp;
      xp->l = child;
      x->r = n->r;
      n->r->p = x;
      target = xp;
      left=true;
    } else{
      target = x;
      left = false;
    }
    goto fixup;
  }
  if (parent) {
    if (parent->l == n) {
      parent->l = child;
      left = true;
    } else {
      parent->r = child;
      left = false;
    }
  } else
    r->root = child;
  if (child)
    child->p = parent;

fixup: 
  if (r->root)
    avl_fixup_del(r, target, left);
  n->p=n->l=n->r=0;
}

