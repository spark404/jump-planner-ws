/** $Id: redblack.h,v 1.1 2008/07/22 11:34:47 cvsd Exp $
 *  
 * Red-Black tree header
 */

typedef enum color { 
	RB_RED, 
	RB_BLACK 
} color_t;

typedef struct redblacknode {
  void *data; // content of this node
  double key; // key value of this node
  color_t color;

  struct redblacknode *parent;
  struct redblacknode *left;
  struct redblacknode *right;
} redblacknode_t;
  
typedef struct redblack {
  redblacknode_t *top;
} redblack_t;

redblack_t *redblack_init();
void redblack_addnode(redblack_t *redblack, void *data, double key);
void redblack_deletenode(redblack_t *redblack, double key);
void *redblack_searchnode(redblack_t *redblack, double key);
void redblack_destroy(redblack_t *redblack, int freedata);
