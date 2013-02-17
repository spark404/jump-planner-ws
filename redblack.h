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
