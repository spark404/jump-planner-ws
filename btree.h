/** $Id: btree.h,v 1.1 2008/07/18 07:13:57 cvsd Exp $
 *  
 * B-Tree header
 */

typedef struct btreenode {
  void *data; // content of this node
  double key; // key value of this node

  struct btreenode *parent;
  struct btreenode *left;
  struct btreenode *right;
} btreenode_t;
  
typedef struct btree {
  int koffie;
  btreenode_t *top;
} btree_t;

btree_t *btree_init();
void btree_addnode(btree_t *btree, void *data, double key);
void btree_deletenode(btree_t *btree, double key);
void *btree_searchnode(btree_t *btree, double key);
void btree_destroy(btree_t *btree, int freedata);
