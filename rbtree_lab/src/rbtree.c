#include "rbtree.h"
#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  // TODO: initialize struct if needed
  p->nil = (node_t *)malloc(sizeof(node_t));
  p->nil->color = RBTREE_BLACK;
  p->root = p->nil;
  return p;
}

void postorder_traverse(rbtree *t, node_t *node)
{
  if (node == t->nil) return;
  postorder_traverse(t, node->left);
  postorder_traverse(t, node->right);
  free(node);
}

void delete_rbtree(rbtree *t)
{
  // TODO: reclaim the tree nodes's memory
  postorder_traverse(t, t->root);
  free(t->nil);
  free(t);
}

void left_rotate(rbtree *t, node_t *x)
{
  node_t *y = x->right;
  x->right = y->left;
  if (x->right != t->nil)
    x->right->parent = x;
  y->parent = x->parent;
  // x의 부모가 nil 이면 루트
  if (x->parent == t->nil)
    t->root = y;
  // 아니면 왼쪽자식인지?
  else if (x == x->parent->left)
    x->parent->left = y;
  // 오른쪽 자식인지?
  else
    x->parent->right = y;
  y->left = x;
  x->parent = y;
}

void right_rotate(rbtree *t, node_t *y)
{
  node_t *x = y->left;
  y->left = x->right;
  if (y->left != t->nil)
    y->left->parent = y;
  x->parent = y->parent;
  // y의 부모가 nil 이면 루트
  if (y->parent == t->nil)
    t->root = x;
  // 아니면 왼쪽자식인지?
  else if (y == y->parent->left)
    y->parent->left = x;
  // 오른쪽 자식인지?
  else
    y->parent->right = x;
  x->right = y;
  y->parent = x;
}

void rbtree_insert_fixup(rbtree *t, node_t *z)
{
  while (z->parent->color == RBTREE_RED)
  {
    if (z->parent == z->parent->parent->left)
    {
      node_t *y = z->parent->parent->right;
      // 케이스 1. z의 부모와 삼촌 y가 모두 레드
      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      {
        // 케이스 2. z의 삼촌 y가 블랙이며 z가 오른쪽 자식
        // 이때 좌회전을 통해 케이스 3으로 진입가능
        if (z == z->parent->right)
        {
          z = z->parent;
          left_rotate(t, z);
        }
        // 케이스 3. z의 삼촌 y가 블랙이며 z가 왼쪽 자식
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        right_rotate(t, z->parent->parent);
      }
    }
    else
    {
      node_t *y = z->parent->parent->left;
      // 케이스 1. z의 부모와 삼촌 y가 모두 레드
      if (y->color == RBTREE_RED)
      {
        z->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        z = z->parent->parent;
      }
      else
      {
        // 케이스 2. z의 삼촌 y가 블랙이며 z가 왼쪽 자식
        // 이때 우회전을 통해 케이스 3으로 진입가능
        if (z == z->parent->left)
        {
          z = z->parent;
          right_rotate(t, z);
        }
        // 케이스 3. z의 삼촌 y가 블랙이며 z가 오른쪽 자식
        z->parent->color = RBTREE_BLACK;
        z->parent->parent->color = RBTREE_RED;
        left_rotate(t, z->parent->parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *z = (node_t*)malloc(sizeof(node_t));
  z->color = RBTREE_RED;
  z->key = key;
  z->left = t->nil;
  z->right = t->nil;

  node_t *cur = t->root;
  node_t *pre = t->nil;
  while (cur != t->nil)
  {
    pre = cur;
    if (z->key < cur->key)
      cur = cur->left;
    else
      cur = cur->right;
  }
  z->parent = pre;
  if (pre == t->nil)
    t->root = z;
  else if (z->key < pre->key)
    pre->left = z;
  else
    pre->right = z;

  rbtree_insert_fixup(t, z);
  return z;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  node_t *cur = t->root;
  while (cur != t->nil)
  {
    if (key < cur->key)
      cur = cur->left;
    else if (key > cur->key)
      cur = cur->right;
    else
      break;
  }
  if (cur == t->nil) return NULL;
  return cur;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  if (t->root == t->nil) return NULL;
  node_t *cur = t->root;
  while (cur->left != t->nil)
    cur = cur->left;
  return cur;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  if (t->root == t->nil) return NULL;
  node_t *cur = t->root;
  while (cur->right != t->nil)
    cur = cur->right;
  return cur;
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v)
{
  if (u->parent == t->nil)
    t->root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;
  v->parent = u->parent;
}

void rbtree_delete_fixup(rbtree *t, node_t *x)
{
  while (x != t->root && x->color == RBTREE_BLACK)
  {
    if (x == x->parent->left)
    {
      node_t *w = x->parent->right;
      // 케이스 1. x의 형제 w가 레드
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      // 케이스 2. x의 형제 w가 블랙이고 w의 자식이 모두 블랙
      else if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        // 케이스 3. x의 형제 w가 블랙이고 w의 왼쪽 자식은 레드, 오른쪽 자식은 블랙
        // 우회전을 통해 케이스 4로 진입가능
        if (w->right->color == RBTREE_BLACK)
        {
          w->left->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          right_rotate(t, w);
          w = x->parent->right;
        }
        // 케이스 4. x의 형제 w는 블랙이고 w의 오른쪽 자식은 레드
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;
      }
    }
    else
    {
      node_t *w = x->parent->left;
      // 케이스 1. x의 형제 w가 레드
      if (w->color == RBTREE_RED)
      {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      // 케이스 2. x의 형제 w가 블랙이고 w의 자식이 모두 블랙
      else if (w->left->color == RBTREE_BLACK && w->right->color == RBTREE_BLACK)
      {
        w->color = RBTREE_RED;
        x = x->parent;
      }
      else
      {
        // 케이스 3. x의 형제 w가 블랙이고 w의 오른쪽 자식은 레드, 왼쪽 자식은 블랙
        // 좌회전을 통해 케이스 4로 진입가능
        if (w->left->color == RBTREE_BLACK)
        {
          w->right->color = RBTREE_BLACK;
          w->color = RBTREE_RED;
          left_rotate(t, w);
          w = x->parent->left;
        }
        // 케이스 4. x의 형제 w는 블랙이고 w의 왼쪽 자식은 레드
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;
      }
    }
  }
  x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *z) {
  // TODO: implement erase
  node_t *x = t->nil;
  int orginal_color = z->color;
  // 자식이 없거나 오른쪽만 있는 경우
  if (z->left == t->nil)
  {
    x = z->right;
    rbtree_transplant(t, z, z->right);
  }
  // 왼쪽 자식만 있는 경우
  else if (z->right == t->nil)
  {
    x = z->left;
    rbtree_transplant(t, z, z->left);
  }
  // 자식이 둘다 있으면 계승시키기
  else
  {
    // y 는 후계자
    node_t *y = z->right;
    while (y->left != t->nil)
      y = y->left;
    orginal_color = y->color;
    x = y->right;
    if (y != z->right)
    {
      rbtree_transplant(t, y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }
    else
      x->parent = y;
    rbtree_transplant(t, z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }
  if (orginal_color == RBTREE_BLACK)
    rbtree_delete_fixup(t, x);
  free(z);
  return 0;
}

void inorder_traverse(const rbtree *t, node_t *node, key_t *arr, int *idx)
{
  if (node == t->nil) return;
  inorder_traverse(t, node->left, arr, idx);
  arr[(*idx)++] = node->key;
  inorder_traverse(t, node->right, arr, idx);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) 
{
  // TODO: implement to_array
  int idx = 0;
  inorder_traverse(t, t->root, arr, &idx);
  return 0;
}