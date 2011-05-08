/*
 * Copyright (C) 2004 Laurent Mazet
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; If not, see <http://www.gnu.org/licenses/>.
 */

/* 2004-01-25
 *   initial release
 */

#include <stdio.h>
#include <stdlib.h>

#include "xmltree.h"

element *new_element () {

  element *new;
  new = (element *) malloc (sizeof(element));
  if (!new)
    perror("xml: no enough memory for new_element()\n");

  new->next = new->child = NULL;

  new->def_value = value_undef;
  new->const_value = const_undef;
  new->scalar_value = 0;
  new->string_value = NULL;

  new->name = NULL;
  new->length = new->rows = new->columns = new->nb_elements = 0;

  return new;
}

element *new_next (element *pred) {

  element *new = new_element();

  if (pred)
    pred->next = new; 

  return new;
}

element *new_child (element *father) {

  element *new = new_element();

  if (father)
    father->child = new; 

  return new;
}

void free_element (element *root) {

  if (!root)
    return;

  if (root->next)
    free_element (root->next);
  if (root->child)
    free_element (root->child);

  if (root->string_value)
    free (root->string_value);
  if (root->name)
    free (root->name);

  free (root);
}

void print_level(int l) {
  int i;
  for (i=0; i<l; i++)
    printf ("  ");
}

void print_element (element *root, int l) {

  if (!root)
    return;

  if (root->name) {
    print_level(l);
    printf ("name: %s\n", root->name);
  }

  print_level(l);
  switch (root->def_value) {
  case value_scalar:
    printf ("scalar: ");
    switch (root->const_value) {
    case const_true: printf ("true\n"); break;
    case const_false: printf ("false\n"); break;
    case const_inf: printf ("inf\n"); break;
    case const_neginf: printf ("neginf\n"); break;
    case const_nan: printf ("nan\n"); break;
    case const_na: printf ("na\n"); break;
    case const_undef: printf ("%f\n", root->scalar_value); break;
    }
    break;
  case value_data:
    printf ("data:\n");
    break;
  case value_complex:
    printf ("complex:\n");
    break;
  case value_string:
    printf ("string (%d): %s\n", root->length, root->string_value);
    break;
  case value_array:
    printf ("array (%d):\n", root->rows);
    break;
  case value_matrix:
    printf ("matrix (%d, %d):\n", root->rows, root->columns);
    break;
  case value_structure:
    printf ("structure:\n");
    break;
  case value_list:
    printf ("list (%d):\n", root->length);
    break;
  case value_cell:
    printf ("cell (%d, %d):\n", root->rows, root->columns);
    break;
  default:
    printf ("???:\n");
  }

  if (root->child) {
    print_level(l);
    printf ("child:\n");
    print_element (root->child, l+1);
  }

  if (root->next) {
    print_level(l);
    printf ("next:\n");
    print_element (root->next, l);
  }
}

list *new_list(list *father) {

  list *new;
  new = (list *) malloc (sizeof(list));
  if (!new)
    perror("xml: no enough memory for new_list()\n");

  new->prev = father;

  new->root = NULL;

  return new;
}

list *pop_list(list *child) {
  list *father;

  father = child->prev;
  free (child);

  return father;
}


