/** $Id: redblack.c,v 1.1 2008/07/22 11:34:47 cvsd Exp $
 *
 * Btree implementation
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "redblack.h"


void redblack_internal_add(redblacknode_t **treenode, redblacknode_t *newnode) {
  if ((*treenode) == NULL) {
    /* case 1 : empty tree */
    (*treenode) = newnode;
    newnode->left = NULL;
    newnode->right = NULL;
#if defined(DEBUG)
    fprintf(stderr, "adding new node (key=%f)\n", newnode->key);
    if (newnode->parent != NULL)
      fprintf(stderr, "  my parent is %f\n", newnode->parent->key);
#endif
  }
  else if (newnode->key < (*treenode)->key) {
    /* case 2 : new value is smaller then current node */
#if defined(DEBUG)
    fprintf(stderr, "adding node (key=%f) left of %f \n", 
	    newnode->key, (*treenode)->key);
#endif
    newnode->parent = (*treenode);
    redblack_internal_add(&((*treenode)->left), newnode);
  }
  else {
    /* case 3 : new value is larger than or equal to current node */
#if defined(DEBUG)
    fprintf(stderr, "adding node (key=%f) right of %f \n", 
	    newnode->key, (*treenode)->key);
#endif
    newnode->parent = (*treenode);
    redblack_internal_add(&((*treenode)->right), newnode);
  }
}
  
redblacknode_t *redblack_internal_searchnode(redblacknode_t *node, double key) {
  if (node == NULL) {
#if defined (DEBUG)
    fprintf(stderr, "key %lf not found\n", key);
#endif
    return NULL;
  }
  else if (node->key == key) {
#if defined (DEBUG)
    fprintf(stderr, "key %lf found\n", key);
#endif
    return node;
  }
  else if (key < node->key )
    return redblack_internal_searchnode(node->left, key);
  else 
    return redblack_internal_searchnode(node->right, key);
}
  
void redblack_internal_destroy(redblacknode_t *node, int freedata) {
  if (node->left != NULL)
    redblack_internal_destroy(node->left, freedata);
  if (node->right != NULL)
    redblack_internal_destroy(node->right, freedata);
  
  redblacknode_t *temp = node;
  node = NULL;
  if (freedata)
    free (node->data);
  free(temp);
}
  
/* external funcions */
redblack_t *redblack_init() {
  redblack_t *redblack;
  redblack = (redblack_t *)malloc(sizeof(redblack_t));
  redblack->top = NULL;
  
  return redblack;
}

void redblack_addnode(redblack_t *redblack, void *data, double key) {
  redblacknode_t *newnode = (redblacknode_t *)malloc(sizeof(redblacknode_t));
  newnode->parent = NULL;
  newnode->left = NULL;
  newnode->right = NULL;
  newnode->key = key;
  newnode->data = data;
  redblack_internal_add(&(redblack->top), newnode);
}

/**
 * destroy the tree and free the allocated
 * resources.
 * paramters:
 *  redblack    : the redblack to destroy
 *  freedata : if != 0 attempt to free() data field
 */
void redblack_destroy(redblack_t *redblack, int freedata) {
  redblack_internal_destroy(redblack->top, freedata);
}

void redblack_deletenode(redblack_t *redblack, double key) {
  fprintf (stderr, "not implemented yet\n");
}

void *redblack_searchnode(redblack_t *redblack, double key) {
  redblacknode_t *result = redblack_internal_searchnode(redblack->top, key);
  if (result != NULL ) {
    return result->data;
  }
  else {
    return NULL;
  }
}
