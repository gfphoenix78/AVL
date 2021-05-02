#include "rbtree.h"
#include <assert.h>
#define assert_imply(p, q) assert(!(p) || (q))

static inline void __rb_link_left(struct rb_node *p, struct rb_node *l)
{
  p->l = l;
  l->p = p;
}
static inline void __rb_link_right(struct rb_node *p, struct rb_node *r)
{
  p->r = r;
  r->p = p;
}
static inline void __rb_link_parent_to_child(struct rb_root *root, struct rb_node *x, struct rb_node *c)
{
  struct rb_node *parent = x->p;
  if (parent) {
    if (parent->l == x)
      parent->l = c;
    else
      parent->r = c;
  } else
    root->root = c;
}

static inline void __rb_link_parent(struct rb_root *root, struct rb_node *x, struct rb_node *child)
{
  __rb_link_parent_to_child(root, x, child);
  if (child)
    child->p = x->p;
}
static inline void __left_rotate(struct rb_node *n, struct rb_root *root)
{
  struct rb_node *parent = n->p;
  struct rb_node *right = n->r;
  if ((n->r = right->l))
    n->r->p = n;

  __rb_link_parent_to_child(root, n, right);
  right->p = parent;
  n->p = right;
  right->l = n;
}
static inline void __right_rotate(struct rb_node *n, struct rb_root *root)
{
  struct rb_node *parent = n->p;
  struct rb_node *left = n->l;
  if ((n->l = left->r))
    n->l->p = n;

  __rb_link_parent_to_child(root, n, left);
  left->p = parent;
  n->p = left;
  left->r = n;
}
static void rb_fixup_insert(struct rb_root *root, struct rb_node *x)
{
  struct rb_node *p;
  struct rb_node *gp;
  struct rb_node *tmp;

  while ((p = x->p) && !p->black) {
    gp = p->p;
    assert(gp && gp->black);
    if (gp->l == p) {
      // parent is the left child
      tmp = gp->r;
      if (tmp && !tmp->black) {
        p->black = tmp->black = true;
        gp->black = false;
        x = gp;
        continue;
      }
      if (p->r == x) {
        __left_rotate(p, root);
        p = x;
      }

      gp->black = false;
      p ->black = true;
      __right_rotate(gp, root);
      break;
    } else {
      // parent is the right child
      assert(gp->r == p);
      tmp = gp->l;
      if (tmp && !tmp->black) {
        p->black = tmp->black = true;
        gp->black = false;
        x = gp;
        continue;
      }
      if (p->l == x) {
        __right_rotate(p, root);
        p = x;
      }

      gp->black = false;
      p ->black = true;
      __left_rotate(gp, root);
      break;
    }
  }
  root->root->black = true;
}
// the subtree(node) is double black
static void rb_fixup_erase(struct rb_root *root, struct rb_node *p, struct rb_node *node)
{
  struct rb_node *sibling;
  struct rb_node *tmp;
  struct rb_node *s1, *s2;
  assert(p->l == node || p->r == node);
  assert(!node);
  while(p) {
    if (p->l == node) {
      sibling = p->r;
      assert(sibling);
      if (!sibling->black) {
        assert(p->black);
        p->black = false;
        sibling->black = true;
        __left_rotate(p, root);
        sibling = p->r;
        // fall-through
      }
      assert(sibling->black);
      s2 = sibling->r;
      if (!s2 || s2->black) {
        s1 = sibling->l;
        if (!s1 || s1->black) {
          sibling->black = false;
          if (!p || p->black) {
            node = p;
            p = node->p;
            continue;
          } else {
            p->black = true;
            return;
          }
        }
        // s1 is red, s2 is black
        assert(s1 && !s1->black);
        assert_imply(s2, s2->black);
        __right_rotate(sibling, root);
        sibling = s1;
        assert(p->r == sibling);
        // fall-through
      }
      assert(sibling);
      sibling->black = p->black;
      p->black = sibling->r->black = true;
      __left_rotate(p, root);
      break;
    } else {
      assert(p->r == node);
      sibling = p->l;
      assert(sibling);
      if (!sibling->black) {
          assert(p->black);
          p->black = false;
          sibling->black = true;
          __right_rotate(p, root);
          sibling = p->l;
          // fall-through
      }
      assert(sibling->black);
      s1 = sibling->l;
      if (!s1 || s1->black) {
        s2 = sibling->r;
        if (!s2 || s2->black) {
          sibling->black = false;
          if (!p || p->black) {
            node = p;
            p = node->p;
            continue;
          } else {
            p->black = true;
            return;
          }
        }
        // s2 is red, s1 is black
        assert(s2 && !s2->black);
        assert_imply(s1, s1->black);
        __left_rotate(sibling, root);
        sibling = s2;
        assert(p->l == sibling);
        // fall-through
      }
      assert(sibling);
      sibling->black = p->black;
      p->black = sibling->l->black = true;
      __right_rotate(p, root);
      break;
    }
  }
  assert_imply(root->root, root->root->black);
}


int rb_insert(struct rb_root *root, struct rb_node *x)
{
  struct rb_node *parent = 0;
  struct rb_node **n = &root->root;
  x->l = x->r = 0;
  x->black = false;

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
  rb_fixup_insert(root, x);
  return 0;
}

struct rb_node *rb_search(struct rb_root *r, int key)
{
  struct rb_node *n = r->root;
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

void rb_delete_node(struct rb_root *root, struct rb_node *node)
{
  struct rb_node *leaf;
  struct rb_node *L, *R, *P;
  struct rb_node *Y, *YP;
  bool is_y_red;

  L = node->l;
  R = node->r;
  P = node->p;
  
  // case 1
  if (!L || !R) {
    leaf = L ? L : R;
    __rb_link_parent(root, node, leaf);
    if (!node->black) {
      assert(!leaf);
      return;
    }
    // node is black
    assert_imply(leaf, !leaf->black);
    if (leaf) {
      assert(!leaf->black);
      assert(!leaf->l && !leaf->r);
      leaf->black = true;
      return;
    }
    if (P)
      rb_fixup_erase(root, P, leaf);
    return;
  }

  assert(L && R);
  Y = R;
  while (Y->l)
    Y = Y->l;
  YP = Y->p;
  leaf = Y->r;
  assert_imply(leaf, !leaf->black && !leaf->l && !leaf->r);
  is_y_red = !Y->black;
  // case 2
  if (YP == node) {
    assert(node->p == P);
    __rb_link_left(Y, L);
    __rb_link_parent(root, node, Y);
    Y->black = node->black;
    if (leaf) {
        leaf->black = true;
        return;
    }
    if (is_y_red)
      return;

    rb_fixup_erase(root, Y, leaf);
    return;
  }

  // case 3
  __rb_link_left(Y, L);
  __rb_link_right(Y, R);
  __rb_link_parent(root, node, Y);
  YP->l = leaf;
  if (leaf)
    leaf->p = YP;

  // replace the color
  Y->black = node->black;
  if (is_y_red) {
    assert(!leaf);
    return;
  }
  // Y is black
  assert_imply(leaf, !leaf->black);
  if (leaf) {
    assert(!leaf->black);
    assert(!leaf->l && !leaf->r);
    leaf->black = true;
    return;
  }
  rb_fixup_erase(root, YP, leaf);
}
