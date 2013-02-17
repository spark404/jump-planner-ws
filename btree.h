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
