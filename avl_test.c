#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>
#include "avl.h"

// n is not nil
bool verify_avl(struct avl_node *n, int *H) {
  int nl, nr;
    int h;
  nl = nr = 0;

  if (n->l) {
    if (!verify_avl(n->l, &nl))
      goto bad;
  }
  if (n->r) {
    if (!verify_avl(n->r, &nr))
      goto bad;
  }
  h = nl - nr;
  if (h < -1 || h > 1)
    goto bad;
    if (h != n->balance) {
        printf("[%d] bad balance: L=%d R=%d, P=%d\n", n->val, nl, nr, n->balance);
        goto bad;
    }
  *H = (nl < nr ? nr : nl) + 1;

    return true;
bad:
   return false;
}
bool verify_avltree(struct avl_root *root)
{
  int H;
  if (root->root && !verify_avl(root->root, &H))
    return false;
  return true;
}


#define NR  (10  * 1000)
//#define verify_avltree(root) true
struct avl_node x[NR];
int main(int ac, const char *av[]) {
  struct avl_root root[1] = { NULL, };
  //    int del_order[] = {0,1,2,3,4,5};
  int i;
  int N = 10;
  bool ok;
repeat:
  srand(time(0));
  for (i = 0; i < NR; i++) {
    int e;
again: 
    x[i].val = rand() % 1000000;

    e = avl_insert(root, x + i);
    if (e == -1)
      goto again;

    ok = verify_avltree(root);
    if (!ok)
      printf("bad avl\n");
  }
  printf("insert ok\n");
  for (i = 0; i < NR; i++) {
    avl_del_node(root, x+i);
    ok = verify_avltree(root);
    if(!ok)
      printf("bad avl = %d\n",i);
  }
  printf("delete ok\n");
  assert(root[0].root == NULL);

  if (N > 0) {
    N--;
    goto repeat;
  }
  return 0;
}

