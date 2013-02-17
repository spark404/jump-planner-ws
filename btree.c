/**
 * Copyright 2008 Hugo Trippaers <opensource@strocamp.net>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "btree.h"


void btree_internal_add(btreenode_t **treenode, btreenode_t *newnode) {
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
    btree_internal_add(&((*treenode)->left), newnode);
  }
  else {
    /* case 3 : new value is larger than or equal to current node */
#if defined(DEBUG)
    fprintf(stderr, "adding node (key=%f) right of %f \n", 
	    newnode->key, (*treenode)->key);
#endif
    newnode->parent = (*treenode);
    btree_internal_add(&((*treenode)->right), newnode);
  }
}
  
btreenode_t *btree_internal_searchnode(btreenode_t *node, double key) {
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
    return btree_internal_searchnode(node->left, key);
  else 
    return btree_internal_searchnode(node->right, key);
}
  
void btree_internal_destroy(btreenode_t *node, int freedata) {
  if (node->left != NULL)
    btree_internal_destroy(node->left, freedata);
  if (node->right != NULL)
    btree_internal_destroy(node->right, freedata);
  
  btreenode_t *temp = node;
  node = NULL;
  if (freedata)
    free (node->data);
  free(temp);
}
  
/* external funcions */
btree_t *btree_init() {
  btree_t *btree;
  btree = (btree_t *)malloc(sizeof(btree_t));
  btree->top = NULL;
  btree->koffie = 10;
  
  return btree;
}

void btree_addnode(btree_t *btree, void *data, double key) {
  btreenode_t *newnode = (btreenode_t *)malloc(sizeof(btreenode_t));
  newnode->parent = NULL;
  newnode->left = NULL;
  newnode->right = NULL;
  newnode->key = key;
  newnode->data = data;
  btree_internal_add(&(btree->top), newnode);
}

/**
 * destroy the tree and free the allocated
 * resources.
 * paramters:
 *  btree    : the btree to destroy
 *  freedata : if != 0 attempt to free() data field
 */
void btree_destroy(btree_t *btree, int freedata) {
  btree_internal_destroy(btree->top, freedata);
}

void btree_deletenode(btree_t *btree, double key) {
  fprintf (stderr, "not implemented yet\n");
}

void *btree_searchnode(btree_t *btree, double key) {
  btreenode_t *result = btree_internal_searchnode(btree->top, key);
  if (result != NULL ) {
    return result->data;
  }
  else {
    return NULL;
  }
}
