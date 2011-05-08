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

/*
  xmltree structure
 */

#if !defined(__XMLTREE_H__)
#define __XMLTREE_H__

typedef enum {
  value_undef = 0,
  value_scalar,
  value_complex,
  value_string,
  value_array,
  value_matrix,
  value_structure,
  value_list,
  value_cell,
  value_data } t_value;

typedef enum {
  const_undef = 0,
  const_true,
  const_false,
  const_inf,
  const_neginf,
  const_na,
  const_nan } t_const;

typedef struct _element {
  struct _element *next;
  struct _element *child;

  /* values */
  t_value def_value;
  t_const const_value;
  double scalar_value;
  char *string_value;

  /* parameters */
  char *name;
  int length;
  int rows;
  int columns;

  /* check */
  int nb_elements;

} element;
  
typedef struct _list {
  struct _list *prev;

  element **root;
} list;

element *new_element ();
element *new_next (element *pred);
element *new_child (element *father);
void free_element (element *root);
void print_element (element *root, int l);

list *new_list(list *father);
list *pop_list(list *child);

#endif /* __XMLTREE_H__ */
