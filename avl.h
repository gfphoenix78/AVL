#ifndef  _AVL_H
#define  _AVL_H

struct bst_node {
        struct bst_node *p;  // parent
        struct bst_node *l;   // left child
        struct bst_node *r;  // right child
};

typedef int (*bst_cmp_t)(const struct bst_node *x, const struct bst_node *y);
struct bst_root {
  struct bst_node *root;
  bst_cmp_t cmp_func;
};

struct avl_node {
        struct avl_node *p;  // parent
        struct avl_node *l;   // left child
        struct avl_node *r;  // right child
        int balance;  		// balance : -1, 0 or 1, 
        int val;  //key for demo
};
typedef int (*avl_cmp_t)(const struct avl_node *x, const struct avl_node *y);

struct avl_root {
  struct avl_node *root;
  avl_cmp_t cmp_func;
};

static inline void avl_init(struct avl_root *r) {
        r->root = 0;
}

int avl_insert(struct avl_root *root, struct avl_node *x);
struct avl_node *avl_search(struct avl_root *r, int key);
void avl_del_node(struct avl_root *r, struct avl_node *n) ;
struct avl_node *avl_delete(struct avl_root *r, int val);
#endif
